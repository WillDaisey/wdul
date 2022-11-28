// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include <cstdint>
#include <Windows.h>

namespace wdul
{
	// Returns the current value of the performance counter. See QueryPerformanceCounter.
	[[nodiscard]] inline std::int64_t get_performance_counts() noexcept
	{
		LARGE_INTEGER li;
		// QueryPerformanceCounter never fails on WinXP or later.
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	// Returns the frequency of the performance counter in counts per second. See QueryPerformanceFrequency.
	[[nodiscard]] inline std::int64_t get_performance_counts_per_sec() noexcept
	{
		LARGE_INTEGER li;
		// QueryPerformanceFrequency never fails on WinXP or later.
		QueryPerformanceFrequency(&li);
		return li.QuadPart;
	}
}
