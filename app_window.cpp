// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/app_window.hpp"
#include "include/wdul/system_resource.hpp"
#include "include/wdul/display.hpp"

namespace wdul
{
	app_window::~app_window()
	{
		destroy();
	}

	void app_window::register_class(_In_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ClassName, _In_opt_ HICON const IconHandle, _In_opt_ HICON const IconHandleSm)
	{
		WNDCLASSEXW wc{};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.hInstance = InstanceHandle;
		wc.lpszClassName = ClassName;
		wc.lpfnWndProc = window_procedure;
		wc.hCursor = load_oem_cursor(oem_cursor::arrow);
		wc.hIcon = IconHandle;
		wc.hIconSm = IconHandleSm;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		auto const atomWindowClass = RegisterClassExW(&wc);
		if (atomWindowClass == 0)
		{
			throw_last_error();
		}
		mWindowClass = window_class(InstanceHandle, atomWindowClass);
	}

	void app_window::create(std::int32_t const ShowCommand, _In_opt_z_ wchar_t const* const WindowName, menu_bar_handle&& MenuBar)
	{
		destroy();
		if (!mHandler)
		{
			mHandler.reset(new app_window_handler());
		}
		mHwnd = CreateWindowExW(
			WS_EX_APPWINDOW,
			MAKEINTRESOURCEW(mWindowClass.get_atom()),
			WindowName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			MenuBar.detach(),
			mWindowClass.get_hinstance(),
			this
		);
		if (!mHwnd)
		{
			auto const lastError = GetLastError();
			check_procedure(); // Check if the window procedure stored an exception which explains why CreateWindow failed.
			throw_win32(lastError, "CreateWindowExW failed"); // Or throw the error set by CreateWindow.
		}
		// hwndGuard ensures the window is destroyed in the event of an exception.
		auto hwndGuard = finally([&]() { if (mHwnd) { WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(DestroyWindow(mHwnd), == 0); } });

		mDpi = static_cast<float>(GetDpiForWindow(mHwnd));
		mClientSizeInPixels = get_window_client_size(mHwnd);
		mClientSizeInDips = pixel_to_dip(mClientSizeInPixels, mDpi);

		ShowWindow(mHwnd, ShowCommand);

		check_procedure(); // Ensure that no exceptions were stored from the window procedure (e.g. through WM_SIZE) before returning.

		// Do not destroy the window when successfully leaving the constructor.
		hwndGuard.revoke();
	}

	void app_window::destroy() noexcept
	{
		if (mHwnd)
		{
			DestroyWindow(mHwnd);
		}
	}

	bool app_window::toggle_fullscreen()
	{
		auto const style = get_window_data(mHwnd, window_data_offset::style);

		if (is_fullscreen())
		{
			// Transition to windowed mode.

			set_window_data(mHwnd, window_data_offset::style, style | WS_OVERLAPPEDWINDOW);
			check_bool(SetWindowPlacement(mHwnd, &mWindowPlacementRestore));
			check_bool(SetWindowPos(mHwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED));

			return false; // Return false to indicate the window is now windowed.
		}
		else
		{
			// Transition to fullscreen mode.

			// Store the window placement so that the window can be restored when leaving fullscreen.
			mWindowPlacementRestore.length = sizeof(mWindowPlacementRestore);
			check_bool(GetWindowPlacement(mHwnd, &mWindowPlacementRestore));

			// Get information about the monitor the window occupies.
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			check_bool(GetMonitorInfoW(MonitorFromWindow(mHwnd, MONITOR_DEFAULTTOPRIMARY), &mi));

			// Remove the WS_OVERLAPPEDWINDOW style.
			set_window_data(mHwnd, window_data_offset::style, style & ~WS_OVERLAPPEDWINDOW);

			// Resize the window so that it fills the monitor display.
			auto& rc = mi.rcMonitor;
			check_bool(SetWindowPos(mHwnd, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED));

			return true; // Return true to indicate the window is now fullscreen.
		}
	}

	bool app_window::is_fullscreen() const
	{
		// Get the monitor which has the largest area of intersection with the window.
		auto const monitor = MonitorFromWindow(mHwnd, MONITOR_DEFAULTTONULL);
		if (!monitor)
		{
			// The window is not fullscreen if it does not intersect any display monitor.
			return false;
		}

		// Get information about the monitor.
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		check_bool(GetMonitorInfoW(monitor, &mi));

		// Get the window rectangle in screen coordinates.
		RECT windowRect;
		check_bool(GetWindowRect(mHwnd, &windowRect));

		// Return true if and only if the window rectangle compares equal to the monitor rectangle.
		auto& a = mi.rcMonitor;
		auto& b = windowRect;
		return (a.left == b.left) && (a.top == b.top) && (a.right == b.right) && (a.bottom == b.bottom);
	}

	void app_window::check_procedure() const
	{
		if (mWndProcException)
		{
			std::rethrow_exception(mWndProcException);
		}
	}

	LONG_PTR __stdcall app_window::window_procedure(HWND const Hwnd, std::uint32_t const Msg, std::uintptr_t const Wparam, LONG_PTR const Lparam) noexcept
	{
		auto self = reinterpret_cast<app_window*>(get_window_data(Hwnd, window_data_offset::userdata));
		if (!self)
		{
			if (Msg != WM_CREATE)
			{
				return DefWindowProcW(Hwnd, Msg, Wparam, Lparam);
			}

			auto const params = reinterpret_cast<CREATESTRUCTW*>(Lparam);
			self = static_cast<app_window*>(params->lpCreateParams);
			try
			{
				set_window_data(Hwnd, window_data_offset::userdata, reinterpret_cast<std::intptr_t>(self));
			}
			catch (...)
			{
				self->mWndProcException = std::current_exception();
				return -1;
			}

			return 0;
		}

		try
		{
			switch (Msg)
			{
			case WM_PAINT:
			{
				window_paint_scope ps(Hwnd);
				self->mHandler->on_paint(ps);
				return 0;
			}

			case WM_SIZE:
				self->mClientSizeInPixels = get_window_client_size(self->mHwnd);
				self->mClientSizeInDips = pixel_to_dip(self->mClientSizeInPixels, self->mDpi);
				self->mHandler->resized(wm_size(Wparam, Lparam));
				return 0;

			case WM_DPICHANGED:
			{
				auto const args = wm_dpichanged(Wparam, Lparam);
				self->mDpi = static_cast<float>(args.dpi.y);
				self->mClientSizeInDips = pixel_to_dip(self->mClientSizeInPixels, self->mDpi);
				args.set_suggested_rect(self->mHwnd);
				app_window_event ev(app_window_event_type::dpi_changed);
				self->mHandler->on_event(ev);
				return 0;
			}

			case WM_MOUSEMOVE:
			{
				wm_mousemove args(Wparam, Lparam);
				self->mMousePosInPixels = args.mouse_pos;
				self->mMousePosInDips = pixel_to_dip(args.mouse_pos, self->mDpi);
				self->mHandler->mouse_moved(args);
				return 0;
			}

			case WM_LBUTTONDOWN:
				self->mHandler->on_mouse_button_down(mouse_button::left, wm_lbuttondown(Wparam, Lparam));
				return 0;

			case WM_LBUTTONUP:
				self->mHandler->on_mouse_button_up(mouse_button::left, wm_lbuttonup(Wparam, Lparam));
				return 0;

			case WM_RBUTTONDOWN:
				self->mHandler->on_mouse_button_down(mouse_button::right, wm_rbuttondown(Wparam, Lparam));
				return 0;

			case WM_RBUTTONUP:
				self->mHandler->on_mouse_button_up(mouse_button::right, wm_rbuttonup(Wparam, Lparam));
				return 0;

			case WM_MBUTTONDOWN:
				self->mHandler->on_mouse_button_down(mouse_button::middle, wm_mbuttondown(Wparam, Lparam));
				return 0;

			case WM_MBUTTONUP:
				self->mHandler->on_mouse_button_up(mouse_button::middle, wm_mbuttonup(Wparam, Lparam));
				return 0;

			case WM_XBUTTONDOWN:
			{
				wm_xbuttondown args(Wparam, Lparam);
				if (args.xbutton == 1)
				{
					self->mHandler->on_mouse_button_down(mouse_button::x1, generic_mouse_args::init(args.keys_down, args.mouse_pos));
				}
				else if (args.xbutton == 2)
				{
					self->mHandler->on_mouse_button_down(mouse_button::x2, generic_mouse_args::init(args.keys_down, args.mouse_pos));
				}
				return 0;
			}

			case WM_XBUTTONUP:
			{
				wm_xbuttonup args(Wparam, Lparam);
				if (args.xbutton == 1)
				{
					self->mHandler->on_mouse_button_up(mouse_button::x1, generic_mouse_args::init(args.keys_down, args.mouse_pos));
				}
				else if (args.xbutton == 2)
				{
					self->mHandler->on_mouse_button_up(mouse_button::x2, generic_mouse_args::init(args.keys_down, args.mouse_pos));
				}
				return 0;
			}

			case WM_KEYDOWN:
				self->mHandler->on_key_down(wm_keydown(Wparam, Lparam));
				return 0;

			case WM_KEYUP:
				self->mHandler->on_key_up(wm_keyup(Wparam, Lparam));
				return 0;

			case WM_SYSKEYDOWN:
			{
				wm_syskeydown args(Wparam, Lparam);
				if (self->mHandler->on_key_down(args))
				{
					return 0;
				}
				return DefWindowProcW(Hwnd, Msg, Wparam, Lparam);
			}

			case WM_SYSKEYUP:
			{
				wm_syskeyup args(Wparam, Lparam);
				if (self->mHandler->on_key_up(args))
				{
					return 0;
				}
				return DefWindowProcW(Hwnd, Msg, Wparam, Lparam);
			}

			case WM_MENUCHAR:
			{
				auto const result = self->mHandler->on_menu_char(wm_menuchar(Wparam, Lparam));
				return wm_menuchar::make_return_code(result.cmd, result.item);
			}
			// Sent when the user selects a command item from a menu, when a control sends a notification message to its parent window, or when an accelerator keystroke is translated.
			case WM_COMMAND:
			{
				wm_command args(Wparam, Lparam);
				if (args.code == 0) // menu
				{
					app_window_event_context<app_window_event_type::menu> ctx;
					ctx.id = args.id;
					app_window_event ev(app_window_event_type::menu, &ctx);
					self->mHandler->on_event(ev);
				}
				else if (args.code == 1) // accelerator
				{
					app_window_event_context<app_window_event_type::accelerator> ctx;
					ctx.id = args.id;
					app_window_event ev(app_window_event_type::accelerator, &ctx);
					self->mHandler->on_event(ev);
				}
				else // control
				{
				}
				return 0;
			}

			case WM_CLOSE:
			{
				app_window_event_context<app_window_event_type::close_request> ctx;
				app_window_event ev(app_window_event_type::close_request, &ctx);
				self->mHandler->on_event(ev);
				if (ctx.destroy)
				{
					DestroyWindow(Hwnd);
				}
				return 0;
			}

			case WM_DESTROY:
				self->mHwnd = nullptr;
				PostQuitMessage(0);
				return 0;

			default:
				return DefWindowProcW(Hwnd, Msg, Wparam, Lparam);
			}
		}
		catch (...)
		{
			self->mWndProcException = std::current_exception();
			DestroyWindow(Hwnd);
			return DefWindowProcW(Hwnd, Msg, Wparam, Lparam);
		}
	}
}
