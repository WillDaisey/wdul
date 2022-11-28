// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once

#include <cstdlib>
#include <type_traits>
#include <concepts>
#include <utility>
#include <cstdint>
#include <string_view>
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


// WDUL_DEBUG_SWITCH expands to DebugCase if _DEBUG is defined, and expands to ReleaseCase if _DEBUG is not defined.
#ifdef _DEBUG
#define WDUL_DEBUG_SWITCH(DebugCase, ReleaseCase) DebugCase
#else
#define WDUL_DEBUG_SWITCH(DebugCase, ReleaseCase) ReleaseCase
#endif

// WDUL_ASSERT_MSG causes the program to terminate with a message if the given expression evaluates to false.
// If _DEBUG is not defined, WDUL_ASSERT_MSG expands to nothing.
// Expr: The expression to evaluate.
// Msg: A pointer to a null-terminated char sequence containing the message, or nullptr.
#define WDUL_ASSERT_MSG(Expr, Msg) WDUL_DEBUG_SWITCH((void)( (!!(Expr)) || (::wdul::impl::assertion_error(__func__, #Expr, Msg), 0) ), )

// Same as calling WDUL_ASSERT_MSG(Expr, nullptr).
#define WDUL_ASSERT(Expr) WDUL_ASSERT_MSG(Expr, nullptr)

// Outputs a warning debug string.
// If _DEBUG is not defined, WDUL_WARN does not expand to anything.
// Module: A pointer to a null-terminated ANSI string corresponding to the program or library from which the error originated, or nullptr.
// Msg: A pointer to a null-terminated ANSI string containing the warning message, or nullptr.
#define WDUL_WARN(Module, Msg) WDUL_DEBUG_SWITCH(::wdul::impl::debug_output(Module, __func__, Msg, ::wdul::impl::severity::warn), )

// Outputs an informational debug string.
// If _DEBUG is not defined, WDUL_INFO does not expand to anything.
// Module: A pointer to a null-terminated ANSI string corresponding to the program or library from which the message originated, or nullptr.
// Msg: A pointer to a null-terminated ANSI string containing the informational message, or nullptr.
#define WDUL_INFO(Module, Msg) WDUL_DEBUG_SWITCH(::wdul::impl::debug_output(Module, __func__, Msg, ::wdul::impl::severity::info), )

// Writes an error message to debug output and causes a breakpoint.
// If _DEBUG is not defined, WDUL_ERRMSG does not expand to anything.
// Module: A pointer to a null-terminated ANSI string corresponding to the program or library from which the message originated, or nullptr.
// Msg: A pointer to a null-terminated ANSI string containing a message explaining the error, or nullptr.
#define WDUL_ERRMSG(Module, Msg) \
	WDUL_DEBUG_SWITCH(::wdul::impl::debug_output(Module, __func__, Msg, ::wdul::impl::severity::error), )

namespace wdul
{
	// Implementation namespace.
	namespace impl
	{
#ifdef _DEBUG
		enum class severity
		{
			info,
			warn,
			error,
		};

		[[noreturn]] void assertion_error(
			_In_opt_z_ char const* const Fn,
			_In_opt_z_ char const* const Expr,
			_In_opt_z_ char const* const Msg
		) noexcept;

		void debug_output(
			_In_opt_z_ char const* const Module,
			_In_opt_z_ char const* const Fn,
			_In_opt_z_ char const* const Msg,
			severity const Sev
		) noexcept;
#endif
	}

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

	// If X is negative, returns -1.
	// If X is positive, returns 1.
	// If X is zero, returns zero.
	template <class T>
	[[nodiscard]] constexpr int signum(T const& X) noexcept(noexcept((T(0) < X) - (X < T(0))))
	{
		return (T(0) < X) - (X < T(0));
	}

	template <class T>
	class vec2
	{
	public:
		using value_type = T;

		value_type x;
		value_type y;

		constexpr vec2() noexcept = default;

		constexpr vec2(value_type const& X, value_type const& Y) noexcept : x(X), y(Y)
		{
		}

		constexpr vec2(vec2 const& Other) noexcept :
			x(Other.x), y(Other.y)
		{
		}

		template <class OtherT>
		constexpr explicit vec2(vec2<OtherT> const& Other) noexcept :
			x(static_cast<value_type const&>(Other.x)), y(static_cast<value_type const&>(Other.y))
		{
		}
	};

	template <class T>
	constexpr vec2<T> operator-(vec2<T> const& Other) noexcept
	{
		return { -Other.x, -Other.y };
	}

	template <class T>
	constexpr vec2<T> operator+(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return { Lhs.x + Rhs.x, Lhs.y + Rhs.y };
	}

	template <class T>
	constexpr vec2<T> operator-(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return { Lhs.x - Rhs.x, Lhs.y - Rhs.y };
	}

	template <class T>
	constexpr vec2<T> operator*(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return { Lhs.x * Rhs.x, Lhs.y * Rhs.y };
	}

	template <class T>
	constexpr vec2<T> operator/(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return { Lhs.x / Rhs.x, Lhs.y / Rhs.y };
	}

	template <class T>
	constexpr vec2<T>& operator+=(vec2<T>& Lhs, vec2<T> const& Rhs) noexcept
	{
		Lhs.x += Rhs.x;
		Lhs.y += Rhs.y;
		return Lhs;
	}

	template <class T>
	constexpr vec2<T>& operator-=(vec2<T>& Lhs, vec2<T> const& Rhs) noexcept
	{
		Lhs.x -= Rhs.x;
		Lhs.y -= Rhs.y;
		return Lhs;
	}

	template <class T>
	constexpr vec2<T>& operator*=(vec2<T>& Lhs, vec2<T> const& Rhs) noexcept
	{
		Lhs.x *= Rhs.x;
		Lhs.y *= Rhs.y;
		return Lhs;
	}

	template <class T>
	constexpr vec2<T>& operator/=(vec2<T>& Lhs, vec2<T> const& Rhs) noexcept
	{
		Lhs.x /= Rhs.x;
		Lhs.y /= Rhs.y;
		return Lhs;
	}

	template <class T>
	constexpr bool operator==(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return (Lhs.x == Rhs.x) && (Lhs.y == Rhs.y);
	}

	template <class T>
	constexpr bool operator>(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return (Lhs.x > Rhs.x) && (Lhs.y > Rhs.y);
	}

	template <class T>
	constexpr bool operator<(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return (Lhs.x < Rhs.x) && (Lhs.y < Rhs.y);
	}

	template <class T>
	constexpr bool operator>=(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return (Lhs.x >= Rhs.x) && (Lhs.y >= Rhs.y);
	}

	template <class T>
	constexpr bool operator<=(vec2<T> const& Lhs, vec2<T> const& Rhs) noexcept
	{
		return (Lhs.x <= Rhs.x) && (Lhs.y <= Rhs.y);
	}

	using vec2i = vec2<std::int32_t>;
	using vec2u = vec2<std::uint32_t>;
	using vec2f = vec2<float>;

	template <class T>
	class vec3
	{
	public:
		using value_type = T;

		value_type x;
		value_type y;
		value_type z;

		constexpr vec3() noexcept = default;

		constexpr vec3(value_type const& X, value_type const& Y, value_type const& Z) noexcept : x(X), y(Y), z(Z)
		{
		}

		constexpr vec3(vec3 const& Other) noexcept :
			x(Other.x), y(Other.y), z(Other.z)
		{
		}

		template <class OtherT>
		explicit vec3(vec3<OtherT> const& Other) noexcept :
			x(static_cast<value_type const&>(Other.x)), y(static_cast<value_type const&>(Other.y)), z(static_cast<value_type const&>(Other.z))
		{
		}
	};

	template <class T>
	constexpr vec3<T> operator-(vec3<T> const& Other) noexcept
	{
		return { -Other.x, -Other.y, -Other.z };
	}

	template <class T>
	constexpr vec3<T> operator+(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return { Lhs.x + Rhs.x, Lhs.y + Rhs.y, Lhs.z + Rhs.z };
	}

	template <class T>
	constexpr vec3<T> operator-(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return { Lhs.x - Rhs.x, Lhs.y - Rhs.y, Lhs.z - Rhs.z };
	}

	template <class T>
	constexpr vec3<T> operator*(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return { Lhs.x * Rhs.x, Lhs.y * Rhs.y, Lhs.z * Rhs.z };
	}

	template <class T>
	constexpr vec3<T> operator/(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return { Lhs.x / Rhs.x, Lhs.y / Rhs.y, Lhs.z / Rhs.z };
	}

	template <class T>
	constexpr vec3<T>& operator+=(vec3<T>& Lhs, vec3<T> const& Rhs) noexcept
	{
		Lhs.x += Rhs.x;
		Lhs.y += Rhs.y;
		Lhs.z += Rhs.z;
		return Lhs;
	}

	template <class T>
	constexpr vec3<T>& operator-=(vec3<T>& Lhs, vec3<T> const& Rhs) noexcept
	{
		Lhs.x -= Rhs.x;
		Lhs.y -= Rhs.y;
		Lhs.z -= Rhs.z;
		return Lhs;
	}

	template <class T>
	constexpr vec3<T>& operator*=(vec3<T>& Lhs, vec3<T> const& Rhs) noexcept
	{
		Lhs.x *= Rhs.x;
		Lhs.y *= Rhs.y;
		Lhs.z *= Rhs.z;
		return Lhs;
	}

	template <class T>
	constexpr vec3<T>& operator/=(vec3<T>& Lhs, vec3<T> const& Rhs) noexcept
	{
		Lhs.x /= Rhs.x;
		Lhs.y /= Rhs.y;
		Lhs.z /= Rhs.z;
		return Lhs;
	}

	template <class T>
	constexpr bool operator==(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return (Lhs.x == Rhs.x) && (Lhs.y == Rhs.y) && (Lhs.z == Rhs.z);
	}

	template <class T>
	constexpr bool operator>(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return (Lhs.x > Rhs.x) && (Lhs.y > Rhs.y) && (Lhs.z > Rhs.z);
	}

	template <class T>
	constexpr bool operator<(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return (Lhs.x < Rhs.x) && (Lhs.y < Rhs.y) && (Lhs.z < Rhs.z);
	}

	template <class T>
	constexpr bool operator>=(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return (Lhs.x >= Rhs.x) && (Lhs.y >= Rhs.y) && (Lhs.z >= Rhs.z);
	}

	template <class T>
	constexpr bool operator<=(vec3<T> const& Lhs, vec3<T> const& Rhs) noexcept
	{
		return (Lhs.x <= Rhs.x) && (Lhs.y <= Rhs.y) && (Lhs.z <= Rhs.z);
	}

	using vec3i = vec3<std::int32_t>;
	using vec3u = vec3<std::uint32_t>;
	using vec3f = vec3<float>;

	template <class T>
	class rect
	{
	public:
		using value_type = T;

		value_type left;
		value_type top;
		value_type right;
		value_type bottom;

		constexpr rect() noexcept = default;

		constexpr rect(value_type const& Left, value_type const& Top, value_type const& Right, value_type const& Bottom) noexcept :
			left(Left), top(Top), right(Right), bottom(Bottom)
		{
		}

		constexpr rect(vec2<value_type> const& UpperLeft, vec2<value_type> const& LowerRight) noexcept :
			left(UpperLeft.x), top(UpperLeft.y), right(LowerRight.x), bottom(LowerRight.y)
		{
		}

		template <class OtherT>
		explicit rect(rect<OtherT> const& Other) noexcept :
			left(static_cast<value_type const&>(Other.left)), top(static_cast<value_type const&>(Other.top)),
			right(static_cast<value_type const&>(Other.right)), bottom(static_cast<value_type const&>(Other.bottom))
		{
		}

		[[nodiscard]] auto upper_left() const noexcept
		{
			return vec2<value_type>(left, top);
		}

		[[nodiscard]] auto upper_right() const noexcept
		{
			return vec2<value_type>(right, top);
		}

		[[nodiscard]] auto lower_left() const noexcept
		{
			return vec2<value_type>(left, bottom);
		}

		[[nodiscard]] auto lower_right() const noexcept
		{
			return vec2<value_type>(right, bottom);
		}

		[[nodiscard]] auto width() const noexcept
		{
			return right - left;
		}

		[[nodiscard]] auto height() const noexcept
		{
			return bottom - top;
		}
	};

	template <class T>
	constexpr rect<T> operator+(rect<T> const& Lhs, rect<T> const& Rhs) noexcept
	{
		return { Lhs.left + Rhs.left, Lhs.top + Rhs.top, Lhs.right + Rhs.right, Lhs.bottom + Rhs.bottom };
	}

	template <class T>
	constexpr rect<T> operator-(rect<T> const& Lhs, rect<T> const& Rhs) noexcept
	{
		return { Lhs.left - Rhs.left, Lhs.top - Rhs.top, Lhs.right - Rhs.right, Lhs.bottom - Rhs.bottom };
	}

	template <class T>
	constexpr rect<T> operator*(rect<T> const& Lhs, rect<T> const& Rhs) noexcept
	{
		return { Lhs.left * Rhs.left, Lhs.top * Rhs.top, Lhs.right * Rhs.right, Lhs.bottom * Rhs.bottom };
	}

	template <class T>
	constexpr rect<T> operator/(rect<T> const& Lhs, rect<T> const& Rhs) noexcept
	{
		return { Lhs.left / Rhs.left, Lhs.top / Rhs.top, Lhs.right / Rhs.right, Lhs.bottom / Rhs.bottom };
	}

	template <class T>
	constexpr rect<T>& operator+=(rect<T>& Lhs, rect<T> const& Rhs) noexcept
	{
		Lhs.left += Rhs.left;
		Lhs.top += Rhs.top;
		Lhs.right += Rhs.right;
		Lhs.bottom += Rhs.bottom;
		return Lhs;
	}

	template <class T>
	constexpr rect<T>& operator-=(rect<T>& Lhs, rect<T> const& Rhs) noexcept
	{
		Lhs.left -= Rhs.left;
		Lhs.top -= Rhs.top;
		Lhs.right -= Rhs.right;
		Lhs.bottom -= Rhs.bottom;
		return Lhs;
	}

	template <class T>
	constexpr rect<T>& operator*=(rect<T>& Lhs, rect<T> const& Rhs) noexcept
	{
		Lhs.left *= Rhs.left;
		Lhs.top *= Rhs.top;
		Lhs.right *= Rhs.right;
		Lhs.bottom *= Rhs.bottom;
		return Lhs;
	}

	template <class T>
	constexpr rect<T>& operator/=(rect<T>& Lhs, rect<T> const& Rhs) noexcept
	{
		Lhs.left /= Rhs.left;
		Lhs.top /= Rhs.top;
		Lhs.right /= Rhs.right;
		Lhs.bottom /= Rhs.bottom;
		return Lhs;
	}

	template <class T>
	constexpr bool operator==(rect<T> const& Lhs, rect<T> const& Rhs) noexcept
	{
		return (Lhs.left == Rhs.left) && (Lhs.top == Rhs.top) &&
			(Lhs.right == Rhs.right) && (Lhs.bottom == Rhs.bottom);
	}

	template <class T>
	[[nodiscard]] constexpr bool has(vec2<T> const& Vec2, std::type_identity_t<T> const& Value) noexcept
	{
		return (Vec2.x == Value) || (Vec2.y == Value);
	}

	template <class T>
	[[nodiscard]] constexpr bool has(vec3<T> const& Vec3, std::type_identity_t<T> const& Value) noexcept
	{
		return (Vec3.x == Value) || (Vec3.y == Value) || (Vec3.z == Value);
	}

	template <class T>
	[[nodiscard]] constexpr bool has(rect<T> const& Rc, std::type_identity_t<T> const& Value) noexcept
	{
		return (Rc.left == Value) || (Rc.top == Value) || (Rc.right == Value) || (Rc.bottom == Value);
	}
	
	template <class T>
	[[nodiscard]] constexpr bool contains(rect<T> const& Container, vec2<T> const& Pt) noexcept
	{
		return Pt.x >= Container.left && Pt.y >= Container.top && Pt.x <= Container.right && Pt.y <= Container.bottom;
	}

	template <class T>
	[[nodiscard]] constexpr bool contains(rect<T> const& Container, rect<T> const& Rc) noexcept
	{
		return Rc.left >= Container.left && Rc.top >= Container.top && Rc.right <= Container.right && Rc.bottom <= Container.bottom;
	}

	using recti = rect<std::int32_t>;
	using rectu = rect<std::uint32_t>;
	using rectf = rect<float>;
}
