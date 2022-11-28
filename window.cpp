// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/window.hpp"
#include "include/wdul/window_message.hpp"

namespace wdul
{
	[[nodiscard]] std::intptr_t get_window_data_or_terminate(_In_ HWND const Hwnd, window_data_offset const Idx) noexcept
	{
		SetLastError(0);
		auto const value = GetWindowLongPtrW(Hwnd, to_underlying(Idx));
		std::uint32_t code;
		if (value == 0 && (code = GetLastError()) != 0)
		{
#ifdef _DEBUG
			try
			{
				throw_win32(code, "get_window_data_or_terminate failed");
			}
			catch (std::exception const& e)
			{
				OutputDebugStringA(e.what());
				OutputDebugStringA("\n");
			}
#endif
			std::terminate();
		}
		return value;
	}

	void wm_dpichanged::set_suggested_rect(_In_ HWND Hwnd) const
	{
		RECT const& rc = *suggested_rect;
		if (SetWindowPos(Hwnd, nullptr,
			rc.left, rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			SWP_NOZORDER | SWP_NOACTIVATE) == 0)
		{
			throw_last_error();
		}
	}
}
