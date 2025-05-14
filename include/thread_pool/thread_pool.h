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
#include "../clock.h"
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
			worker(sync_queue<task>& task_list) : _task_list(task_list) {}
			~worker() { stop(); }

			void start() {
				if (_thread.joinable()) return;
				_thread = jthread(
					[this](stop_token stop_token) {
						while (!stop_token.stop_requested()) {
							clock clock;
							clock.begin();
							auto task_opt = _task_list.pop();
							_average_get_work_cost = clock.end();
							if (!task_opt.has_value()) continue;
							auto& task			   = task_opt.value();
							if (task.function) task.function();
							if (_idle && !stop_token.stop_requested()) {
								unique_lock idle_lock(_idle_mtx);
								_idle_cv.wait(idle_lock,
											  [this, stop_token] { return !_idle || stop_token.stop_requested(); });
							}
						}
					},
					_thread.get_stop_token());
			}

			void stop() {
				_thread.request_stop();
				if (_thread.joinable()) _thread.join();
			}

			inline void active() {
				_idle = false;
				_average_get_work_cost = 0;
				_idle_cv.notify_all();
			}

			inline void idle() { _idle = true; }

			inline u64 average_get_work_cost() { return _average_get_work_cost; }

		   private:
			jthread _thread;
			sync_queue<task>& _task_list;
			condition_variable _idle_cv;
			mutex _idle_mtx;
			atomic_bool _idle{false};
			atomic_u64 _average_get_work_cost{0};
		};
	}  // namespace detail

	class thread_pool {

	   public:
		using ExpansionStrategy =
			func<u32(const u64 /*max_tasks*/, const u64 /*current_tasks*/, const u64 /*current_threads*/)>;

	   public:
		thread_pool() = default;
		~thread_pool() { shutdown(); }

		void startup(const u64 thread_count, const u64 max_task_count, ExpansionStrategy&& expansion_strategy) {
			if (_startup) return;
			_max_task_count		= !max_task_count ? 99 : max_task_count;
			_expansion_strategy = expansion_strategy;

			lock_guard lock(_worker_mtx);
			for (u32 i = 0; i < thread_count; ++i) {
				auto worker = make_unique<detail::worker>(_tasks);
				worker->start();
				_actived_workers.emplace_back(std::move(worker));
			}
			_startup = true;
		}

		void shutdown() {
			if (!_startup) return;
			lock_guard lock(_worker_mtx);
			for (auto& work : _actived_workers)
				work->stop();
			for (auto& work : _idle_workers)
				work->stop();
			_actived_workers.clear();
			_idle_workers.clear();
			_startup = false;
		}

		template <typename Func, typename... Args>
		bool publish(Func&& func, Args&&... args) {
			if (!_startup) return false;
			if (_tasks.size() >= _max_task_count) {
				if (!_Expand()) return false;
			}
			else if (_tasks.size() < _max_task_count / 5) {
				_Shrink();
			}
			detail::task task;
			task.function = [f = std::forward<Func>(func), ... args_captured = std::forward<Args>(args)]() mutable {
				try {
					f(args_captured...);
				}catch(...){}
			};
			return _tasks.push(std::move(task));
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

			if (!publish(std::move(task)))
				promise_ptr->set_exception(std::make_exception_ptr(std::runtime_error("publish task failed")));

			return future_result;
		}

	   private:
		bool _Expand() {
			lock_guard lock(_worker_mtx);
			
			u64 new_thread_count = _expansion_strategy 
				? _expansion_strategy(_max_task_count, _tasks.size(), _actived_workers.size()) 
				: 1;
			
			if (new_thread_count == 0) return false;
			
			u64 max_threads = std::thread::hardware_concurrency() * 2 - 1;
			u64 available_slots = _actived_workers.size() < max_threads 
				? max_threads - _actived_workers.size() 
				: 0;
			
			new_thread_count = std::min(new_thread_count, available_slots);
			if (new_thread_count == 0) return false;
			
			u64 to_create = new_thread_count;
			while (!_idle_workers.empty() && to_create > 0) {
				auto worker = std::move(_idle_workers.back());
				_idle_workers.pop_back();
				worker->active();
				_actived_workers.emplace_back(std::move(worker));
				to_create--;
			}
			
			for (u64 i = 0; i < to_create; ++i) {
				auto worker = make_unique<detail::worker>(_tasks);
				worker->start();
				_actived_workers.emplace_back(std::move(worker));
			}
			return true;
		}

		void _Shrink() {
			lock_guard lock(_worker_mtx);
			for (auto it = _actived_workers.begin(); it != _actived_workers.end();) {
				//make sure at least one worker is active, the first one.
				if (it == _actived_workers.begin())  {
					it++;
					continue;
				}
				auto& worker = *it;
				if (worker->average_get_work_cost() > 60 * 1000) {
					worker->idle();
					_idle_workers.emplace_back(std::move(worker));
					it = _actived_workers.erase(it);
				}
				else it++;
			}
		}

	   private:
		atomic_bool _startup{false};
		atomic_u64 _max_task_count{0};
		ExpansionStrategy _expansion_strategy;
		mutex _worker_mtx;
		vector<detail::worker::ptr> _actived_workers;
		vector<detail::worker::ptr> _idle_workers;
		sync_queue<detail::task> _tasks;
	};
}  // namespace ema
