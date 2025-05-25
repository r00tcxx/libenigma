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
#include <future>
#include <random>
#include "class.h"
#include "container/sync_queue.h"

namespace ema {
	namespace detail {
		class CallableBase {
		   public:
			virtual ~CallableBase() = default;
			virtual void invoke()	= 0;
		};

		template <typename F, typename... Args>
		class CallableImpl : public CallableBase {
			F func;
			std::tuple<Args...> args;

		   public:
			CallableImpl(F&& f, Args&&... a) : func(std::forward<F>(f)), args(std::forward<Args>(a)...) {
			}

			void invoke() override {
				std::apply(std::move(func), std::move(args));
			}
		};

		struct Task {
			std::unique_ptr<CallableBase> callable;

			Task()						 = default;
			Task(const Task&)			 = delete;
			Task& operator=(const Task&) = delete;
			Task(Task&&)				 = default;
			Task& operator=(Task&&)		 = default;

			template <typename F, typename... Args>
			static Task Create(F&& func, Args&&... args) {
				Task task;
				task.callable = std::make_unique<CallableImpl<std::decay_t<F>, std::decay_t<Args>...>>(
					std::forward<F>(func), std::forward<Args>(args)...);
				return task;
			}

			void operator()() {
				if (callable) callable->invoke();
			}
		};

		class Worker : public NoCopyableMoveable {
		   public:
			using Ptr = std::unique_ptr<Worker>;

		   public:
			Worker(std::stop_token stop_token, std::atomic<std::size_t>& counter) : _counter(counter) {
				_thread = std::thread([this, stop_token]() {
					while (!stop_token.stop_requested()) {
						_busy		  = false;
						auto task_opt = _tasks.Pop([&] { return stop_token.stop_requested(); });
						_busy		  = true;
						if (!task_opt.has_value()) continue;
						auto& task = task_opt.value();
						task();
						_counter--;
					}
				});
			}

			~Worker() {
				_tasks.Clear();
				_tasks.Stop();
				if (_thread.joinable()) _thread.join();
			}

			inline bool IsBusy() const {
				return _busy;
			}
			inline auto GetTaskCount() {
				return _tasks.Size();
			}
			inline void push_task(Task&& task) {
				_counter++;
				_tasks.Push(std::move(task));
			}

		   private:
			std::thread _thread;
			std::atomic_bool _busy{false};
			std::atomic<std::size_t>& _counter;
			SyncQueue<Task> _tasks;
		};

	}  // namespace detail

	class ThreadPool {
	   public:
		ThreadPool() = default;
		~ThreadPool() {
			Shutdown();
		}

		inline void Startup(const u64 thread_count, const u64 max_task_count, const bool allow_expand = true) {
			if (_start) return;
			_max_task_count			= !max_task_count ? 999 : max_task_count;
			_allow_expand			= allow_expand;
			const auto create_count = !thread_count ? 1 : thread_count;

			std::lock_guard lock(_worker_mtx);
			for (u64 i = 0; i < create_count; ++i)
				_workers.emplace_back(
					std::move(std::make_unique<detail::Worker>(_stop_source.get_token(), _curr_task_count)));
			_start = true;
		}

		inline void Shutdown() {
			if (!_start) return;
			_stop_source.request_stop();
			//wait worker's thread join finished
			_workers.clear();
			_start = false;
		}

		template <typename Func, typename... Args>
		inline bool Submit(Func&& func, Args&&... args) {
			if (!_start) return false;

			auto task = detail::Task::Create(std::forward<Func>(func), std::forward<Args>(args)...);

			std::lock_guard lock(_worker_mtx);
			if (_curr_task_count > _max_task_count) return false;

			for (auto& worker : _workers) {
				if (!worker->IsBusy()) {
					worker->push_task(std::move(task));
					return true;
				}
			}
			//no worker's idle
			const auto max_thread_count = std::thread::hardware_concurrency() * 2;
			if (!_allow_expand || _workers.size() + 1 > max_thread_count) {
				//cant create worker anymore, routuer task to randmon worker
				constexpr auto random = [](auto from, auto to) -> std::size_t {
					std::random_device rd;
					std::mt19937 gen(rd());
					return std::uniform_int_distribution<std::size_t>(from, to)(gen);
				};
				_workers.at(random(0, _workers.size() - 1))->push_task(std::move(task));
			}
			else {
				auto new_worker = make_unique<detail::Worker>(_stop_source.get_token(), _curr_task_count);
				new_worker->push_task(std::move(task));
				_workers.emplace_back(std::move(new_worker));
			}
			return true;
		}

		template <typename Func, typename... Args>
		inline auto exec(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
			using return_type  = std::invoke_result_t<Func, Args...>;
			auto promise_ptr   = std::make_shared<std::promise<return_type>>();
			auto future_result = promise_ptr->get_future();

			auto task = [promise_ptr, f = std::forward<Func>(func),
						 ... args_captured = std::forward<Args>(args)]() mutable {
				try {
					if constexpr (std::is_void_v<return_type>) {
						f(args_captured...);
						promise_ptr->set_value();
					}
					else {
						promise_ptr->set_value(f(std::forward<Args>(args_captured)...));
					}
				} catch (...) {
					promise_ptr->set_exception(std::current_exception());
				}
			};

			if (!Submit(std::move(task)))
				promise_ptr->set_exception(std::make_exception_ptr(std::runtime_error("submit task failed")));
			return future_result;
		}

		inline std::size_t GetCurrentTaskCount() const {
			return _curr_task_count;
		}
		inline std::size_t GetMaxTaskCount() const {
			return _max_task_count;
		}

	   private:
		std::atomic_bool _allow_expand{true};
		std::atomic_bool _start{false};
		std::atomic<std::size_t> _curr_task_count{0};
		std::atomic<std::size_t> _max_task_count{0};
		std::mutex _worker_mtx;
		std::vector<detail::Worker::Ptr> _workers;
		std::stop_source _stop_source;
	};
}  // namespace ema
