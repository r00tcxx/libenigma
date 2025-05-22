#pragma once
#include "thread.h"
#include "types.h"

namespace ema {
	template <typename T>
	concept promise_type = std::is_move_constructible_v<T> && std::is_move_assignable_v<T>;

	namespace detail {
		template <typename F, typename S>
			requires promise_type<F> && promise_type<S>
		class _shared_state : public NoCopyableNoMoveable {
		   public:
			_shared_state()	 = default;
			~_shared_state() = default;

			inline void set_fail(F&& v) {
				_s_func = move(v);
			}

			inline set_success(S&& v) {
				_f_func = move(v);
			}

			inline void set_result(std::variant<F, S>&& v) {
			}

			inline bool is_finished() {
				return _result.has_value();
			}

		   private:
			optional<variant<F, S>> _result;
			func<void(S&&)> _s_func{nullptr};
			func<void(F&&)> _f_func{nullptr};
		};

		template <typename F, typename S>
			requires promise_type<F> && promise_type<S>
		using _shared_state_ptr = shared_ptr<_shared_state<F, S>>
	}  // namespace detail

	template <typename F, typename S>
		requires promise_type<F> && promise_type<S>
	class future {
		template <typename F1, typename S1>
			requires promise_type<F1> && promise_type<S1>
		friend class promise;

	   public:
		using type = future<F, S>;

	   public:
		inline future(type&& other) {
			_state		 = other._state;
			other._state = nullptr;
		}

		inline future(const type& other) {
			_state = other._state;
		}

		inline type& operator=(type&& other) {
			if (this != &other) {
				_state		 = other._state;
				other._state = nullptr;
			}
			return *this;
		}

		inline type& operator=(const type& other) {
			if (this != &other) {
				_state = other._state;
			}
			return *this;
		}

		~future() {
		}

		inline type& then(func<void(S&&)>&& callback) {
			if (callback) _state->set_success(move(callback));
			return *this;
		}

		inline type& failed(func<void(F&&)>&& callback) {
			if (callback) _state->set_fail(move(callback));
			return *this;
		}

		inline bool is_finished() {
			return _state->is_finished();
		}

	   private:
		future(detail::_shared_state_ptr state) : _state(state) {
			if (!_state) throw std::runtime_error("bad shared state");
		}

	   private:
		detail::_shared_state_ptr _state{nullptr};
	};

	template <typename F, typename S>
	class promise : public no_copyable {
	   public:
		promise() : _state(make_shared<detail::_shared_promise_state>()) {
		}
		promise(promise&& other) {
		}
		~promise() {
		}

		future<F, S> get_future();
		inline void set_failed(F&& v) {
		}
		inline void set_success(S&& v) {
		}

	   private:
		shared_ptr<detail::_shared_promise_state<F, S>> _state;
	};

}  // namespace ema