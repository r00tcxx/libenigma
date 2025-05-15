/*
 * Created on Mon Mar 24 2025
 *
 * The MIT License (MIT)
 * Copyright (c) 2025 enigma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include "../sync_queue.h"
#include "../types.h"

namespace ema {
	namespace detail {
		struct task {
			func<void()> function;
		};

		class worker : public no_cmable {
		   public:
			using ptr = unique_ptr<worker>;

		   public:
			worker(stop_token stop_token, atomic_u64& counter) : _counter(counter) {
				_thread = thread([this, stop_token]() {
					while (!stop_token.stop_requested()) {
						_busy		  = false;
						auto task_opt = _tasks.pop([&] { return stop_token.stop_requested(); });
						_busy		  = true;
						if (!task_opt.has_value()) continue;
						auto& task = task_opt.value();
						if (task.function) task.function();
						_counter--;
					}
				});
			}

			~worker() {
				_tasks.clear();
				_tasks.stop();
				if (_thread.joinable()) _thread.join();
			}

			inline bool is_busy() const { return _busy; }
			inline auto get_task_count() { return _tasks.size(); }
			inline void push_task(task&& task) {
				_counter++;
				_tasks.push(move(task));
			}

		   private:
			thread _thread;
			atomic_bool _busy{false};
			atomic_u64& _counter;
			sync_queue<task> _tasks;
		};

	}  // namespace detail

	class thread_pool {
	   public:
		thread_pool() = default;
		~thread_pool() { shutdown(); }

		void startup(const u64 thread_count, const u64 max_task_count, const bool allow_expand = true) {
			if (_start) return;
			_max_task_count			= !max_task_count ? 999 : max_task_count;
			_allow_expand			= allow_expand;
			const auto create_count = !thread_count ? 1 : thread_count;

			lock_guard lock(_worker_mtx);
			for (u64 i = 0; i < create_count; ++i)
				_workers.emplace_back(move(make_unique<detail::worker>(_stop_source.get_token(), _curr_task_count)));
			_start = true;
		}

		void shutdown() {
			if (!_start) return;
			_stop_source.request_stop();
			//wait worker's thread join finished
			_workers.clear();
			_start = false;
		}

		template <typename Func, typename... Args>
		bool submit(Func&& func, Args&&... args) {
			if (!_start) return false;

			detail::task task;
			task.function = [f = std::forward<Func>(func), ... args_captured = std::forward<Args>(args)]() mutable {
				try {
					f(args_captured...);
				} catch (...) {}
			};

			lock_guard lock(_worker_mtx);
			if (_curr_task_count > _max_task_count) return false;

			for (auto& worker : _workers) {
				if (!worker->is_busy()) {
					worker->push_task(move(task));
					return true;
				}
			}
			//no worker's idle
			const auto max_thread_count = thread::hardware_concurrency() * 2;
			if (!_allow_expand || _workers.size() + 1 > max_thread_count) {
				//cant create worker anymore, routuer task to randmon worker
				_workers.at(random<u64>(0, _workers.size() - 1))->push_task(move(task));
			}
			else {
				auto new_worker = make_unique<detail::worker>(_stop_source.get_token(), _curr_task_count);
				new_worker->push_task(move(task));
				_workers.emplace_back(move(new_worker));
			}
			return true;
		}

		template <typename Func, typename... Args>
		auto exec(Func&& func, Args&&... args) -> future<std::invoke_result_t<Func, Args...>> {
			using return_type  = std::invoke_result_t<Func, Args...>;
			auto promise_ptr   = std::make_shared<promise<return_type>>();
			auto future_result = promise_ptr->get_future();

			auto task = [promise_ptr, f = std::forward<Func>(func),
						 ... args_captured = std::forward<Args>(args)]() mutable {
				try {
					if constexpr (std::is_void_v<return_type>) {
						f(args_captured...);
						promise_ptr->set_value();
					}
					else {
						promise_ptr->set_value(f(args_captured...));
					}
				} catch (...) {
					promise_ptr->set_exception(std::current_exception());
				}
			};

			if (!submit(move(task)))
				promise_ptr->set_exception(std::make_exception_ptr(std::runtime_error("submit task failed")));
			return future_result;
		}

		inline u64 get_curr_task_count() const { return _curr_task_count; }
		inline u64 get_max_task_count() const { return _max_task_count; }

	   private:
		atomic_bool _allow_expand{true};
		atomic_bool _start{false};
		atomic_u64 _curr_task_count{0};
		atomic_u64 _max_task_count{0};
		mutex _worker_mtx;
		vector<detail::worker::ptr> _workers;
		stop_source _stop_source;
	};
}  // namespace ema
