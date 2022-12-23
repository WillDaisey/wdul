// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include <cstdlib>
#include <type_traits>
#include <concepts>
#include <utility>
#include <cstdint>
#include <sal.h>

// WDUL_DECLARE_ENUM_FLAGS defines operator overloads (and the has_flag function) for scoped enumerations designed to be used as flags.
#define WDUL_DECLARE_ENUM_FLAGS(name) \
	inline constexpr name operator | (name const a, name const b) noexcept { return static_cast<name>(static_cast<::std::underlying_type_t<name>>(a) | static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr name operator & (name const a, name const b) noexcept { return static_cast<name>(static_cast<::std::underlying_type_t<name>>(a) & static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr name operator ^ (name const a, name const b) noexcept { return static_cast<name>(static_cast<::std::underlying_type_t<name>>(a) ^ static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr name operator ~ (name const a) noexcept { return static_cast<name>(~static_cast<::std::underlying_type_t<name>>(a)); } \
	inline constexpr name& operator |= (name& a, name const b) noexcept { return (name&)((::std::underlying_type_t<name>&)(a) |= static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr name& operator &= (name& a, name const b) noexcept { return (name&)((::std::underlying_type_t<name>&)(a) &= static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr name& operator ^= (name& a, name const b) noexcept { return (name&)((::std::underlying_type_t<name>&)(a) ^= static_cast<::std::underlying_type_t<name>>(b)); } \
	inline constexpr bool has_flag(name const a, name const b) noexcept { return static_cast<bool>(static_cast<::std::underlying_type_t<name>>(a) & static_cast<::std::underlying_type_t<name>>(b)); }

namespace wdul
{
	// Conceptualises a type which is neither const nor volatile.
	template <class T>
	concept cv_unqualified = !(std::is_const_v<T> || std::is_volatile_v<T>);

	// Conceptualises a type which is neither const nor volatile, and is not a reference.
	template <class T>
	concept cvref_unqualified = cv_unqualified<T> && (!std::is_reference_v<T>);

	// Used to suppress uninitialised variable warnings.
	template <class T>
	void leave_uninitialized(T const&) noexcept {}

	// Converts an enumeration to its underlying type.
	// When C++23 is available, we can use std::to_underlying.
	template <class T>
	[[nodiscard]] constexpr auto to_underlying(T const EnumValue) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(EnumValue);
	}

	// Class whose destructor calls T.
	template <cvref_unqualified T>
	class final_act
	{
		final_act(const final_act&) = delete;
		final_act& operator=(const final_act&) = delete;
		final_act& operator=(final_act&&) = delete;
	public:
		explicit final_act(T const Fn) noexcept : mFn(std::move(Fn)) {}

		final_act(final_act&& Other) noexcept : mFn(std::move(Other.mFn)), mInvoke(std::exchange(Other.mInvoke, false))
		{
		}

		~final_act() noexcept(std::is_nothrow_invocable_v<T>)
		{
			if (mInvoke) mFn();
		}

		void consume() noexcept(std::is_nothrow_invocable_v<T>)
		{
			if (mInvoke)
			{
				mFn();
				mInvoke = false;
			}
		}

		void revoke() noexcept
		{
			mInvoke = false;
		}

		void should_invoke(bool const ShouldInvoke) noexcept
		{
			mInvoke = ShouldInvoke;
		}

		[[nodiscard]] bool should_invoke() const noexcept
		{
			return mInvoke;
		}

	private:
		T mFn;
		bool mInvoke = true;
	};

	// Makes a final_act object.
	template <class T>
	[[nodiscard]] auto finally(T&& fn) noexcept
	{
		return final_act<std::remove_cvref_t<T>>(std::forward<T>(fn));
	}

	// Class whose destructor calls T.
	// Unlike final_act, final_irrevocable_act cannot cancel its invocation to T.
	template <cvref_unqualified T>
	class final_irrevocable_act
	{
		final_irrevocable_act(const final_irrevocable_act&) = delete;
		final_irrevocable_act& operator=(const final_irrevocable_act&) = delete;
		final_irrevocable_act(final_irrevocable_act&&) = delete;
		final_irrevocable_act& operator=(final_irrevocable_act&&) = delete;
	public:
		explicit final_irrevocable_act(T const Fn) noexcept : mFn(std::move(Fn)) {}

		~final_irrevocable_act() noexcept(std::is_nothrow_invocable_v<T>)
		{
			mFn();
		}

	private:
		T mFn;
	};

	// Makes a final_irrevocable_act object.
	template <class T>
	[[nodiscard]] auto finally_always(T&& fn) noexcept
	{
		return final_irrevocable_act<std::remove_cvref_t<T>>(std::forward<T>(fn));
	}
}
