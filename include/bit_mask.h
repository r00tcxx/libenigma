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
#include <type_traits>

namespace ema {
#define BIT_MASK(_BITMASK)                                                                      \
	[[nodiscard]] constexpr _BITMASK operator&(_BITMASK _Left, _BITMASK _Right) noexcept {      \
		using _IntTy = std::underlying_type_t<_BITMASK>;                                        \
		return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) & static_cast<_IntTy>(_Right)); \
	}                                                                                           \
                                                                                                \
	[[nodiscard]] constexpr _BITMASK operator|(_BITMASK _Left, _BITMASK _Right) noexcept {      \
		using _IntTy = std::underlying_type_t<_BITMASK>;                                        \
		return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) | static_cast<_IntTy>(_Right)); \
	}                                                                                           \
                                                                                                \
	[[nodiscard]] constexpr _BITMASK operator^(_BITMASK _Left, _BITMASK _Right) noexcept {      \
		using _IntTy = std::underlying_type_t<_BITMASK>;                                        \
		return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) ^ static_cast<_IntTy>(_Right)); \
	}                                                                                           \
                                                                                                \
	constexpr _BITMASK& operator&=(_BITMASK& _Left, _BITMASK _Right) noexcept {                 \
		return _Left = _Left & _Right;                                                          \
	}                                                                                           \
                                                                                                \
	constexpr _BITMASK& operator|=(_BITMASK& _Left, _BITMASK _Right) noexcept {                 \
		return _Left = _Left | _Right;                                                          \
	}                                                                                           \
                                                                                                \
	constexpr _BITMASK& operator^=(_BITMASK& _Left, _BITMASK _Right) noexcept {                 \
		return _Left = _Left ^ _Right;                                                          \
	}                                                                                           \
                                                                                                \
	[[nodiscard]] constexpr _BITMASK operator~(_BITMASK _Left) noexcept {                       \
		using _IntTy = std::underlying_type_t<_BITMASK>;                                        \
		return static_cast<_BITMASK>(~static_cast<_IntTy>(_Left));                              \
	}

	template <class _Bitmask>
	[[nodiscard]] constexpr bool _Bitmask_includes_any(_Bitmask _Left, _Bitmask _Elements) noexcept {
		return (_Left & _Elements) != _Bitmask{};
	}

	template <class _Bitmask>
	[[nodiscard]] constexpr bool _Bitmask_includes_all(_Bitmask _Left, _Bitmask _Elements) noexcept {
		return (_Left & _Elements) == _Elements;
	}
}  // namespace EMA
