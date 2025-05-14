#pragma once
#include <utility>
#include <optional>
#include <tuple>

namespace ema {
	class Err {
	   public:
		explicit Err(const char* what) : what_(what) {}
		Err() : what_(nullptr) {}
		
		Err(const Err& other) : what_(other.what_) {}
		Err& operator=(const Err& other) {
			if (this != &other) {
				what_ = other.what_;
			}
			return *this;
		}
		
		Err(Err&& other) noexcept : what_(other.what_) {
			other.what_ = nullptr;
		}
		Err& operator=(Err&& other) noexcept {
			if (this != &other) {
				what_ = other.what_;
				other.what_ = nullptr;
			}
			return *this;
		}
		
		const char* what() const { return what_; }
		operator bool() { return nullptr != what_; }

	   private:
		const char* what_{nullptr};
	};

	template <typename T>
	class Result {
	public:
		Result(T&& value) : value_(std::move(value)), err_(nullptr) {}
		Result(Err&& err) : value_(std::nullopt), err_(std::move(err)) {}
		
		bool isOk() const { return !err_ && value_.has_value(); }
		bool isErr() const { return (bool)err_; }
		
		T valueOr(T&& defaultValue) const {
			return value_.value_or(std::forward<T>(defaultValue));
		}
		
		const T& value() const { return value_.value(); }
		T& value() { return value_.value(); }
		
		const Err& error() const { return err_; }
		
		template <std::size_t I>
		auto get() const {
			if constexpr (I == 0) {
				return err_;
			} else if constexpr (I == 1) {
				return value_;
			}
		}
		
		template <std::size_t I>
		auto get() {
			if constexpr (I == 0) {
				return err_;
			} else if constexpr (I == 1) {
				return value_;
			}
		}
		
		operator std::tuple<Err, std::optional<T>>() const {
			return std::make_tuple(err_, value_);
		}
		
	private:
		std::optional<T> value_;
		Err err_;
	};

	template <>
	class Result<void> {
	public:
		Result() : err_(nullptr) {}
		Result(Err&& err) : err_(std::move(err)) {}
		
		bool isOk() const { return !err_; }
		bool isErr() const { return (bool)err_; }
		
		const Err& error() const { return err_; }
		
		template <std::size_t I>
		auto get() const {
			if constexpr (I == 0) {
				return err_;
			} else if constexpr (I == 1) {
				return nullptr;
			}
		}
		
		template <std::size_t I>
		auto get() {
			if constexpr (I == 0) {
				return err_;
			} else if constexpr (I == 1) {
				return nullptr;
			}
		}
		
		operator std::tuple<Err, std::nullptr_t>() const {
			return std::make_tuple(err_, nullptr);
		}
		
	private:
		Err err_;
	};
	
	template <typename T>
	Result<std::decay_t<T>> Ok(T&& value) {
		return Result<std::decay_t<T>>(std::forward<T>(value));
	}
	
	inline Result<void> Ok() {
		return Result<void>();
	}
	
	template <typename T>
	Result<T> Fail(const char* error) {
		return Result<T>(Err(error));
	}
	
	inline Result<void> Fail(const char* error) {
		return Result<void>(Err(error));
	}
	
	template <typename From, typename To, typename Func>
	Result<To> Transform(Result<From>&& result, Func&& transformer) {
		if (result.isErr()) {
			return Fail<To>(result.error().what());
		}
		
		if constexpr (std::is_void_v<To>) {
			transformer(std::move(result.value()));
			return Ok();
		} else {
			return Ok(transformer(std::move(result.value())));
		}
	}
	
	template <typename To, typename Func>
	Result<To> Transform(Result<void>&& result, Func&& transformer) {
		if (result.isErr()) {
			return Fail<To>(result.error().what());
		}
		
		return Ok(transformer());
	}
}

namespace std {
	template <typename T>
	struct tuple_size<ema::Result<T>> : std::integral_constant<size_t, 2> {};

	template <size_t I, typename T>
	struct tuple_element<I, ema::Result<T>> {
		using type = decltype(std::declval<ema::Result<T>>().template get<I>());
	};
}
