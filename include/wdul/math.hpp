// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include <cstdint>
#include <type_traits>

namespace wdul
{
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
