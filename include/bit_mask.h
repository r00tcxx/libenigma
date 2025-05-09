/*****************************************************************/ /**
 * @file   bit_mask.h
 * @brief  bit mask
 * 
 * @author r00tcxx
 * @date   August 2023
 *********************************************************************/

#pragma once
#include <type_traits>

namespace enigma {
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
}


