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
#include <atomic>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <type_traits>

namespace ema {
	using JThread			= std::thread;
	using Thread			= std::jthread;
	using StopSource		= std::stop_source;
	using StopToken			= std::stop_token;
	using ConditionVariable = std::condition_variable;
	using Mutext			= std::mutex;
	using SharedMutex		= std::shared_mutex;

	template <typename T>
	using Atomic = std::atomic<T>;

	template <typename T>
	using UniquePtr = std::unique_lock<T>;

	template <typename T>
	using LockGuard = std::lock_guard<T>;

	template <class... _Mutexes>
	using ScopedLock = std::scoped_lock<_Mutexes...>;

	template <typename T>
	concept FutureType = std::is_void_v<T> || (std::is_move_constructible_v<T> && std::is_move_assignable_v<T>);

	namespace detail {
		template <bool IsVoid, typename T>
		struct _FuncSelector;

		template <typename T>
			requires(!std::is_void_v<T>)
		struct _FuncSelector<false, T> {
			using Type = std::function<void(T&&)>;
		};

		template <typename T>
			requires(std::is_void_v<T>)
		struct _FuncSelector<true, T> {
			using Type = std::function<void()>;
		};

		template <FutureType F, FutureType S>
		class _SharedPromiseState {
		   public:
			static constexpr bool _void_success = std::is_void_v<S>;
			static constexpr bool _void_fail	= std::is_void_v<F>;
			using SuccessFunc					= typename _FuncSelector<_void_success, S>::Type;
			using FailFunc						= typename _FuncSelector<_void_fail, F>::Type;

		   public:
			_SharedPromiseState()									   = default;
			_SharedPromiseState(const _SharedPromiseState&)			   = delete;
			_SharedPromiseState(_SharedPromiseState&&)				   = delete;
			~_SharedPromiseState()									   = default;
			_SharedPromiseState& operator=(const _SharedPromiseState&) = delete;
			_SharedPromiseState& operator=(_SharedPromiseState&&)	   = delete;

			inline void SetFailFunc(FailFunc&& v) {
				if (!v) return;
				_f_func.store(std::make_shared<FailFunc>(std::move(v)));
			}

			inline void SetSuccessFunc(SuccessFunc&& v) {
				if (!v) return;
				_s_func.store(std::make_shared<SuccessFunc>(std::move(v)));
			}

			template <typename... Args>
			inline void SetFailed(Args&&... args) {
				if constexpr (_void_fail) {
					static_assert(sizeof...(args) == 0, "bad argument");
					_is_finished = true;
					if (auto f = _f_func.load()) (*f)();
				}
				else {
					static_assert(sizeof...(args) == 1, "bad argument");
					_is_finished = true;
					if (auto f = _f_func.load()) (*f)(std::forward<Args>(args)...);
				}
			}

			template <typename... Args>
			inline void SetSuccessed(Args&&... args) {
				if constexpr (_void_success) {
					static_assert(sizeof...(args) == 0, "bad argument");
					_is_finished  = true;
					_is_successed = true;
					if (auto f = _s_func.load()) (*f)();
				}
				else {
					static_assert(sizeof...(args) == 1, "bad argument");
					_is_finished  = true;
					_is_successed = true;
					if (auto f = _s_func.load()) (*f)(std::forward<Args>(args)...);
				}
			}

			inline bool IsFinished() {
				return _is_finished.load();
			}

			inline bool IsSuccessed() {
				return _is_successed.load();
			}

		   private:
			std::atomic_bool _is_finished{false}, _is_successed{false};
			std::atomic<std::shared_ptr<SuccessFunc>> _s_func;
			std::atomic<std::shared_ptr<FailFunc>> _f_func;
		};

	}  // namespace detail

	template <FutureType F, FutureType S>
	class Promise;

	template <FutureType F, FutureType S>
	class Future {
		friend Promise;

	   public:
		using Type			  = Future<F, S>;
		using SharedStateType = detail::_SharedPromiseState<F, S>;

	   public:
		Future(Type&& other) {
			_state.store(other._state.load());
			other._state.store(nullptr);
		}

		Future(const Type& other) {
			_state = other._state;
		}

		~Future() {
		}

		inline Type& operator=(Type&& other) {
			if (this != &other) {
				_state.store(other._state.load());
				other._state.store(nullptr);
			}
			return *this;
		}

		inline Type& operator=(const Type& other) {
			if (this != &other) _state.store(other._state.load());
			return *this;
		}

		inline Type& Then(SharedStateType::SuccessFunc&& func) {
			if (func) {
				if (auto s = _state.load()) s->SetSuccessFunc(std::move(func));
			}
			return *this;
		}

		inline Type& Fail(SharedStateType::FailFunc&& func) {
			if (func) {
				if (auto s = _state.load()) s->SetFailFunc(std::move(func));
			}
			return *this;
		}

		inline bool IsFinished() {
			if (auto s = _state.load()) return s->IsFinished();
			return false;
		}

		inline bool IsSuccessed() {
			if (auto s = _state.load()) return s->IsSuccessed();
			return false;
		}

	   private:
		Future(std::shared_ptr<SharedStateType> state) {
			if (!state) throw std::runtime_error("bad shared state");
			_state.store(state);
		}

	   private:
		std::atomic<std::shared_ptr<SharedStateType>> _state;
	};

	template <FutureType F, FutureType S>
	class Promise {
	   public:
		using Type			  = Future<F, S>;
		using SharedStateType = detail::_SharedPromiseState<F, S>;
		using FutureType	  = Future<F, S>;

	   public:
		Promise() {
			_state.store(std::make_shared<SharedStateType>());
		}

		Promise(const Promise&) = delete;

		Promise(Promise&& other) {
			_state.stroe(other._state.load());
			other._state.store(nullptr);
		}

		~Promise() {
		}

		inline Promise& operator=(const Promise&) = delete;
		inline Promise& operator=(Promise&& other) {
			_state.stroe(other._state.load());
			other._state.store(nullptr);
			return *this;
		}

		inline FutureType GetFuture() {
			auto s = _state.load();
			return Future<F, S>(s);
		}

		template <typename... Args>
		inline void SetFailed(Args&&... args) {
			if (auto s = _state.load()) s->SetFailed(std::forward<Args>(args)...);
		}

		template <typename... Args>
		inline void SetSuccessed(Args&&... args) {
			if (auto s = _state.load()) s->SetSuccessed(std::forward<Args>(args)...);
		}

	   private:
		std::atomic<std::shared_ptr<SharedStateType>> _state;
	};

}  // namespace ema