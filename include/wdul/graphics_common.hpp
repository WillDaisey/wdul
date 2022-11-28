// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"

namespace wdul
{
	// Indicates the availability of the graphics device.
	enum class graphics_device_presence : bool
	{
		// The device is available or device loss was not detected.
		ready,

		// The device was lost, invalidating device-dependent resources associated with the device.
		// Handle this by recreating the device and device-dependent resources.
		lost,
	};

	class color4f
	{
	public:
		color4f() noexcept = default;

		constexpr color4f(float const Red, float const Green, float const Blue, float const Alpha = 1.f) noexcept :
			r(Red), g(Green), b(Blue), a(Alpha)
		{
		}

		constexpr explicit color4f(std::uint32_t const Rgb, float const Alpha = 1.0f) noexcept :
			r(static_cast<float>((Rgb& rgb_red_mask) >> rgb_red_shift) / 255.f),
			g(static_cast<float>((Rgb& rgb_green_mask) >> rgb_green_shift) / 255.f),
			b(static_cast<float>((Rgb& rgb_blue_mask) >> rgb_blue_shift) / 255.f),
			a(Alpha)
		{
		}

		float r;
		float g;
		float b;
		float a;

	private:
		static std::uint32_t constexpr rgb_red_shift = 16;
		static std::uint32_t constexpr rgb_green_shift = 8;
		static std::uint32_t constexpr rgb_blue_shift = 0;
		static std::uint32_t constexpr rgb_red_mask = 0xff << rgb_red_shift;
		static std::uint32_t constexpr rgb_green_mask = 0xff << rgb_green_shift;
		static std::uint32_t constexpr rgb_blue_mask = 0xff << rgb_blue_shift;
	};
}
