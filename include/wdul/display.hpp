// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"

namespace wdul
{
	// Pixel (PixelType) -> DIP (float)

	template <std::integral PixelType>
	[[nodiscard]] constexpr float pixel_to_dip(PixelType const Pixel, float const Dpi) noexcept
	{
		return Pixel / (Dpi / 96.f);
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr vec2f pixel_to_dip(vec2<PixelType> const& Pixels, float const Dpi) noexcept
	{
		float const dpiScale = Dpi / 96.f;
		return { static_cast<float>(Pixels.x) / dpiScale, static_cast<float>(Pixels.y) / dpiScale };
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr vec2f pixel_to_dip(vec2<PixelType> const& Pixels, vec2f const& Dpi) noexcept
	{
		return { static_cast<float>(Pixels.x) / (Dpi.x / 96.f), static_cast<float>(Pixels.y) / (Dpi.y / 96.f) };
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr rectf pixel_to_dip(rect<PixelType> const& Pixels, float const Dpi) noexcept
	{
		float const dpiScale = Dpi / 96.f;
		return { static_cast<float>(Pixels.left) / dpiScale, static_cast<float>(Pixels.top) / dpiScale,
			static_cast<float>(Pixels.right) / dpiScale, static_cast<float>(Pixels.bottom) / dpiScale };
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr rectf pixel_to_dip(rect<PixelType> const& Pixels, vec2f const& Dpi) noexcept
	{
		auto const dpiScale = Dpi / vec2f(96.f, 96.f);
		return { static_cast<float>(Pixels.left) / dpiScale.x, static_cast<float>(Pixels.top) / dpiScale.y,
			static_cast<float>(Pixels.right) / dpiScale.x, static_cast<float>(Pixels.bottom) / dpiScale.y };
	}

	// DIP (float) -> pixel (PixelType)

	template <std::integral PixelType>
	[[nodiscard]] constexpr PixelType dip_to_pixel(float const Dip, float const Dpi) noexcept
	{
		return static_cast<PixelType>(Dip * (Dpi / 96.f));
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr vec2<PixelType> dip_to_pixel(vec2f const& Dips, float const Dpi) noexcept
	{
		float const dpiScale = Dpi / 96.f;
		return vec2<PixelType>(static_cast<PixelType>(Dips.x * dpiScale), static_cast<PixelType>(Dips.y * dpiScale));
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr vec2<PixelType> dip_to_pixel(vec2f const& Dips, vec2f const& Dpi) noexcept
	{
		return { static_cast<PixelType>(Dips.x * (Dpi.x / 96.f)), static_cast<PixelType>(Dips.y * (Dpi.y / 96.f)) };
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr rect<PixelType> dip_to_pixel(rectf const& Pixels, float const Dpi) noexcept
	{
		float const dpiScale = Dpi / 96.f;
		return { static_cast<PixelType>(Pixels.left * dpiScale), static_cast<PixelType>(Pixels.top * dpiScale),
			static_cast<PixelType>(Pixels.right * dpiScale), static_cast<PixelType>(Pixels.bottom * dpiScale) };
	}

	template <std::integral PixelType>
	[[nodiscard]] constexpr rect<PixelType> dip_to_pixel(rectf const& Pixels, vec2f const& Dpi) noexcept
	{
		auto const dpiScale = Dpi / vec2f(96.f, 96.f);
		return { static_cast<PixelType>(Pixels.left * dpiScale.x), static_cast<PixelType>(Pixels.top * dpiScale.y),
			static_cast<PixelType>(Pixels.right * dpiScale.x), static_cast<PixelType>(Pixels.bottom * dpiScale.y) };
	}
}
