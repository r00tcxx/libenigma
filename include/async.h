#pragma once
#include "types.h"

namespace ema {
	template <typename F, typename S>
	class async : public no_copyable {
	   public:
		using failed_callback  = func<void(F&&)>;
		using success_callback = func<void(S&&)>;

		async()							   = default;
		async(async&&) noexcept			   = default;
		async& operator=(async&&) noexcept = default;

		template <typename T>
		void failed(T&& v) {
			if (_failed) _failed(std::forward<T>(v));
		}

		template <typename T>
		void successed(T&& v) {
			if (_successed) _successed(std::forward<T>(v));
		}

		async& on_failed(failed_callback cb) {
			_failed = std::move(cb);
			return *this;
		}

		async& on_successed(success_callback cb) {
			_successed = std::move(cb);
			return *this;
		}

	   private:
		failed_callback _failed{nullptr};
		success_callback _successed{nullptr};
	};

	template <typename F>
	class async<F, void> : public no_copyable {
	   public:
		using failed_callback  = func<void(F&&)>;
		using success_callback = func<void()>;

		async()							   = default;
		async(async&&) noexcept			   = default;
		async& operator=(async&&) noexcept = default;

		template <typename T>
		void failed(T&& v) {
			if (_failed) _failed(std::forward<T>(v));
		}

		void successed() {
			if (_successed) _successed();
		}

		async& on_failed(failed_callback cb) {
			_failed = std::move(cb);
			return *this;
		}

		async& on_successed(success_callback cb) {
			_successed = std::move(cb);
			return *this;
		}

	   private:
		failed_callback _failed{nullptr};
		success_callback _successed{nullptr};
	};

	template <typename S>
	class async<void, S> : public no_copyable {
	   public:
		using failed_callback  = func<void()>;
		using success_callback = func<void(S&&)>;

		async()							   = default;
		async(async&&) noexcept			   = default;
		async& operator=(async&&) noexcept = default;

		void failed() {
			if (_failed) _failed();
		}

		template <typename T>
		void successed(T&& v) {
			if (_successed) _successed(std::forward<T>(v));
		}

		async& on_failed(failed_callback cb) {
			_failed = std::move(cb);
			return *this;
		}

		async& on_successed(success_callback cb) {
			_successed = std::move(cb);
			return *this;
		}

	   private:
		failed_callback _failed{nullptr};
		success_callback _successed{nullptr};
	};

	// 特化模板：F 是 void，S 是 void
	template <>
	class async<void, void> : public no_copyable {
	   public:
		using failed_callback  = func<void()>;
		using success_callback = func<void()>;

		async()							   = default;
		async(async&&) noexcept			   = default;
		async& operator=(async&&) noexcept = default;

		void failed() {
			if (_failed) _failed();
		}

		void successed() {
			if (_successed) _successed();
		}

		async& on_failed(failed_callback cb) {
			_failed = std::move(cb);
			return *this;
		}

		async& on_successed(success_callback cb) {
			_successed = std::move(cb);
			return *this;
		}

	   private:
		failed_callback _failed{nullptr};
		success_callback _successed{nullptr};
	};
}  // namespace ema