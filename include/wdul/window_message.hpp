// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"
#include "error.hpp"
#include <bit>
#include <Windows.h>

namespace wdul
{
	namespace impl
	{
		[[nodiscard]] inline constexpr std::int16_t get_x_lparam(std::intptr_t const Lparam) noexcept
		{
			return static_cast<std::int16_t>(LOWORD(Lparam));
		}

		[[nodiscard]] inline constexpr std::int16_t get_y_lparam(std::intptr_t const Lparam) noexcept
		{
			return static_cast<std::int16_t>(HIWORD(Lparam));
		}
	}

	enum class mouse_key_flags : std::uint8_t
	{
		none = 0x0,
		lbutton = MK_LBUTTON,
		rbutton = MK_RBUTTON,
		shift = MK_SHIFT,
		control = MK_CONTROL,
		mbutton = MK_MBUTTON,
		xbutton1 = MK_XBUTTON1,
		xbutton2 = MK_XBUTTON2,
	};
	WDUL_DECLARE_ENUM_FLAGS(mouse_key_flags);

	struct wm_size
	{
		enum class resize_type : std::uint8_t
		{
			restored = SIZE_RESTORED,
			minimized = SIZE_MINIMIZED,
			maxshow = SIZE_MAXSHOW,
			maximized = SIZE_MAXIMIZED,
			maxhide = SIZE_MAXHIDE,
		};

		wm_size() noexcept = default;

		wm_size(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			type(static_cast<resize_type>(Wparam)),
			size(LOWORD(Lparam), HIWORD(Lparam))
		{
		}

		[[nodiscard]] static wm_size init(resize_type const Type, vec2<std::uint16_t> const Size) noexcept
		{
			wm_size v;
			v.type = Type;
			v.size = Size;
			return v;
		}

		resize_type type;
		vec2<std::uint16_t> size;
	};

	struct generic_mouse_args
	{
		generic_mouse_args() noexcept = default;

		generic_mouse_args(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			keys_down(static_cast<mouse_key_flags>(Wparam)),
			mouse_pos(impl::get_x_lparam(Lparam), impl::get_y_lparam(Lparam))
		{
		}

		[[nodiscard]] static generic_mouse_args init(mouse_key_flags const KeysDown, vec2i const& MousePos) noexcept
		{
			generic_mouse_args v;
			v.keys_down = KeysDown;
			v.mouse_pos = MousePos;
			return v;
		}

		mouse_key_flags keys_down;
		vec2i mouse_pos;
	};

	using wm_mousemove = generic_mouse_args;
	using wm_mousehover = generic_mouse_args;
	using wm_lbuttondown = generic_mouse_args;
	using wm_mbuttondown = generic_mouse_args;
	using wm_rbuttondown = generic_mouse_args;
	using wm_lbuttonup = generic_mouse_args;
	using wm_mbuttonup = generic_mouse_args;
	using wm_rbuttonup = generic_mouse_args;
	using wm_lbuttondblclk = generic_mouse_args;
	using wm_mbuttondblkclk = generic_mouse_args;
	using wm_rbuttondblclk = generic_mouse_args;

	struct generic_xbutton_args
	{
		generic_xbutton_args() noexcept = default;

		generic_xbutton_args(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			keys_down(static_cast<mouse_key_flags>(GET_KEYSTATE_WPARAM(Wparam))),
			xbutton(GET_XBUTTON_WPARAM(Wparam)),
			mouse_pos(impl::get_x_lparam(Lparam), impl::get_y_lparam(Lparam))
		{
		};

		[[nodiscard]] static generic_xbutton_args init(mouse_key_flags const KeysDown, std::uint16_t const Xbutton, vec2i const& MousePos) noexcept
		{
			generic_xbutton_args v;
			v.keys_down = KeysDown;
			v.xbutton = Xbutton;
			v.mouse_pos = MousePos;
			return v;
		}

		mouse_key_flags keys_down;
		std::uint16_t xbutton; // Where 1 == XButton1 and 2 == XButton2.
		vec2i mouse_pos;
	};

	using wm_xbuttondown = generic_xbutton_args;
	using wm_xbuttonup = generic_xbutton_args;
	using wm_xbuttondblclk = generic_xbutton_args;

	struct generic_mouse_wheel_args
	{
		static auto constexpr wheel_delta = WHEEL_DELTA;

		generic_mouse_wheel_args() noexcept = default;

		generic_mouse_wheel_args(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			keys_down(static_cast<mouse_key_flags>(GET_KEYSTATE_WPARAM(Wparam))),
			distance(GET_WHEEL_DELTA_WPARAM(Wparam)),
			mouse_pos(impl::get_x_lparam(Lparam), impl::get_y_lparam(Lparam))
		{
		}

		[[nodiscard]] static generic_mouse_wheel_args init(mouse_key_flags const KeysDown, std::int16_t const Distance, vec2i const& MousePos) noexcept
		{
			generic_mouse_wheel_args v;
			v.keys_down = KeysDown;
			v.distance = Distance;
			v.mouse_pos = MousePos;
			return v;
		}

		mouse_key_flags keys_down;
		std::int16_t distance;
		vec2i mouse_pos;
	};

	using wm_mousewheel = generic_mouse_wheel_args;
	using wm_mousehwheel = generic_mouse_wheel_args;

	struct generic_key_args
	{
		generic_key_args() noexcept = default;

		generic_key_args(bool const System, std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			system(System),
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			repeat_count(static_cast<std::uint8_t>(LOWORD(Lparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			context_code = (hi & KF_ALTDOWN) == KF_ALTDOWN;
			prev_state = (hi & KF_REPEAT) == KF_REPEAT;
			up = (hi & KF_UP) == KF_UP;
		}

		[[nodiscard]] static generic_key_args init(bool const System, std::uint8_t const Key, std::uint8_t const RepeatCount, std::uint8_t const ScanCode,
			bool const Extended, bool const ContextCode, bool const PrevState, bool const Up) noexcept
		{
			generic_key_args v;
			v.system = System;
			v.key = Key;
			v.repeat_count = RepeatCount;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.context_code = ContextCode;
			v.prev_state = PrevState;
			v.up = Up;
			return v;
		}

		// This field is true if the key is a system character key, i.e., a character key that is pressed while the ALT key is 
		// down.
		bool system;

		// The character code of the key.
		std::uint8_t key;

		// The repeat count for the current message. This is the number of times the keystroke was auto-repeated as a result
		// of the user holding down the key. If the keystroke is held long enough, multiple messages are sent. However, the repeat
		// count is not cumulative.
		std::uint8_t repeat_count;

		// The scan code. Depends on the OEM.
		std::uint8_t scan_code;

		// Specifies whether the key is an extended key (true) or not (false).
		bool extended;

		// The context code. This field is true if and only if the ALT key is down whilst the key is being pressed or released.
		// For a WM_SYSKEYDOWN or WM_SYSKEYUP message, this field is false if the message was posted to the active window because 
		// no window has the keyboard focus.
		bool context_code;

		// The previous key state. This field is true if and only if the key is down before the message is sent.
		bool prev_state;

		// The transition state. This field is true if the key is being released, or false if the key is being pressed.
		bool up;
	};

	// A subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	struct generic_nonsystem_key_args
	{
		generic_nonsystem_key_args() noexcept = default;

		generic_nonsystem_key_args(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			repeat_count(static_cast<std::uint8_t>(LOWORD(Lparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			prev_state = (hi & KF_REPEAT) == KF_REPEAT;
			up = (hi & KF_UP) == KF_UP;
		}

		[[nodiscard]] static generic_nonsystem_key_args init(std::uint8_t const Key, std::uint8_t const RepeatCount, std::uint8_t const ScanCode,
			bool const Extended, bool const PrevState, bool const Up) noexcept
		{
			generic_nonsystem_key_args v;
			v.key = Key;
			v.repeat_count = RepeatCount;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.prev_state = PrevState;
			v.up = Up;
			return v;
		}

		std::uint8_t key;
		std::uint8_t repeat_count;
		std::uint8_t scan_code;
		bool extended;
		bool prev_state;
		bool up;
	};

	// A subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	struct generic_system_key_args
	{
		generic_system_key_args() noexcept = default;

		generic_system_key_args(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			repeat_count(static_cast<std::uint8_t>(LOWORD(Lparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			context_code = (hi & KF_ALTDOWN) == KF_ALTDOWN;
			prev_state = (hi & KF_REPEAT) == KF_REPEAT;
			up = (hi & KF_UP) == KF_UP;
		}

		[[nodiscard]] static generic_system_key_args init(std::uint8_t const Key, std::uint8_t const RepeatCount, std::uint8_t const ScanCode,
			bool const Extended, bool const ContextCode, bool const PrevState, bool const Up) noexcept
		{
			generic_system_key_args v;
			v.key = Key;
			v.repeat_count = RepeatCount;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.context_code = ContextCode;
			v.prev_state = PrevState;
			v.up = Up;
			return v;
		}

		std::uint8_t key;
		std::uint8_t repeat_count;
		std::uint8_t scan_code;
		bool extended;
		bool context_code;
		bool prev_state;
		bool up;
	};

	// Structure which translates the arguments of a WM_KEYDOWN message.
	//
	// The wm_keydown structure contains a subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	//
	// View the MSDN documentation for the WM_KEYDOWN message.
	struct wm_keydown
	{
		wm_keydown() noexcept = default;

		wm_keydown(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			repeat_count(static_cast<std::uint8_t>(LOWORD(Lparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			prev_state = (hi & KF_REPEAT) == KF_REPEAT;
		};

		[[nodiscard]] static wm_keydown init(std::uint8_t const Key, std::uint8_t const RepeatCount, std::uint8_t const ScanCode,
			bool const Extended, bool const PrevState) noexcept
		{
			wm_keydown v;
			v.key = Key;
			v.repeat_count = RepeatCount;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.prev_state = PrevState;
			return v;
		}

		std::uint8_t key;
		std::uint8_t repeat_count;
		std::uint8_t scan_code;
		bool extended;
		bool prev_state;
	};

	// Structure which translates the arguments of a WM_KEYUP message.
	//
	// The wm_keyup structure contains a subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	//
	// View the MSDN documentation for the WM_KEYUP message.
	struct wm_keyup
	{
		wm_keyup() noexcept = default;

		wm_keyup(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
		}

		[[nodiscard]] static wm_keyup init(std::uint8_t const Key, std::uint8_t const ScanCode, bool Extended) noexcept
		{
			wm_keyup v;
			v.key = Key;
			v.scan_code = ScanCode;
			v.extended = Extended;
			return v;
		}

		std::uint8_t key;
		std::uint8_t scan_code;
		bool extended;
	};

	// Structure which translates the arguments of a WM_SYSKEYDOWN message.
	//
	// The wm_syskeydown structure contains a subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	//
	// View the MSDN documentation for the WM_SYSKEYDOWN message.
	struct wm_syskeydown
	{
		wm_syskeydown() noexcept = default;

		wm_syskeydown(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			repeat_count(static_cast<std::uint8_t>(LOWORD(Lparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			context_code = (hi & KF_ALTDOWN) == KF_ALTDOWN;
			prev_state = (hi & KF_REPEAT) == KF_REPEAT;
		}

		[[nodiscard]] static wm_syskeydown init(std::uint8_t const Key, std::uint8_t const RepeatCount, std::uint8_t const ScanCode,
			bool const Extended, bool const ContextCode, bool const PrevState) noexcept
		{
			wm_syskeydown v;
			v.key = Key;
			v.repeat_count = RepeatCount;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.context_code = ContextCode;
			v.prev_state = PrevState;
			return v;
		}

		std::uint8_t key;
		std::uint8_t repeat_count;
		std::uint8_t scan_code;
		bool extended;
		bool context_code;
		bool prev_state;
	};

	// Structure which translates the arguments of a WM_SYSKEYUP message.
	//
	// The wm_syskeyup structure contains a subset of the fields contained in the generic_key_args structure.
	// For information about a field, refer to the generic_key_args structure.
	//
	// View the MSDN documentation for the WM_SYSKEYUP message.
	struct wm_syskeyup
	{
		wm_syskeyup() noexcept = default;

		wm_syskeyup(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam)))
		{
			auto const hi = HIWORD(Lparam);
			scan_code = LOBYTE(hi);
			extended = (hi & KF_EXTENDED) == KF_EXTENDED;
			context_code = (hi & KF_ALTDOWN) == KF_ALTDOWN;
		}

		[[nodiscard]] static wm_syskeyup init(std::uint8_t const Key, std::uint8_t const ScanCode, bool const Extended, bool const ContextCode) noexcept
		{
			wm_syskeyup v;
			v.key = Key;
			v.scan_code = ScanCode;
			v.extended = Extended;
			v.context_code = ContextCode;
			return v;
		}

		std::uint8_t key;
		std::uint8_t scan_code;
		bool extended;
		bool context_code;

	};

	// Structure which translates the arguments of a WM_DPICHANGED message.
	//
	// The WM_DPICHANGED message is sent when the effective DPI for a window has changed.
	//
	// View the MSDN documentation for the WM_DPICHANGED message.
	struct wm_dpichanged
	{
		wm_dpichanged() noexcept = default;

		wm_dpichanged(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			dpi(LOWORD(Wparam), HIWORD(Wparam)),
			suggested_rect(reinterpret_cast<RECT*>(Lparam))
		{
		}

		[[nodiscard]] static wm_dpichanged init(vec2<std::uint16_t> const Dpi, RECT* const SuggestedRect) noexcept
		{
			wm_dpichanged v;
			v.dpi = Dpi;
			v.suggested_rect = SuggestedRect;
			return v;
		}

		// The new DPI for the window. The X-axis and Y-axis are identical for Windows apps.
		vec2<std::uint16_t> dpi;

		// A pointer to a RECT structure that provides a suggested size and position of the window scaled for the new DPI, where:
		// 1) suggested_rect.left is the suggested position of the left side of the window, in client coordinates.
		// 2) suggested_rect.top is the suggested position of the top of the window, in client coordinates.
		// 3) suggested_rect.right - suggested_rect.left gives the suggested width of the window, in pixels.
		// 4) suggested_rect.bottom - suggested_rect.top gives the suggested height of the window, in pixels. 
		//
		// It is expected that you call the set_suggested_rect member function to reposition and resize the window based on this
		// field.
		RECT* suggested_rect;

		// Uses the RECT structure pointed to by the suggested_rect field to reposition and resize the given window.
		void set_suggested_rect(_In_ HWND const Hwnd) const;
	};

	// Structure which translates the arguments of a WM_INITMENUPOPUP message.
	// The WM_INITMENUPOPUP message is sent when a drop-down menu or submenu is about to become active.
	// View the MSDN documentation for the WM_INITMENUPOPUP message.
	struct wm_initmenupopup
	{
		wm_initmenupopup() noexcept = default;

		wm_initmenupopup(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			hmenu(reinterpret_cast<HMENU>(Wparam)),
			index(LOWORD(Lparam)),
			is_window_menu(static_cast<bool>(HIWORD(Lparam)))
		{
		}

		[[nodiscard]] static wm_initmenupopup init(HMENU const Hmenu, std::uint16_t const Index, bool const IsWindowMenu) noexcept
		{
			wm_initmenupopup v;
			v.hmenu = Hmenu;
			v.index = Index;
			v.is_window_menu = IsWindowMenu;
			return v;
		}

		// A handle to the drop-down menu or submenu.
		HMENU hmenu;

		// The zero-based relative position of the menu item that opens the drop-down menu or submenu.
		std::uint16_t index;

		// This field is true if and only if the menu is the window menu.
		bool is_window_menu;
	};

	// Return codes for WM_MENUCHAR.
	enum class menu_char_cmd : std::uint8_t
	{
		// Informs the system that it should discard the character the user pressed and create a short beep on the system speaker.
		ignore = MNC_IGNORE,

		// Informs the system that it should close the active menu.
		close = MNC_CLOSE,

		// Informs the system that it should choose the item specified in the low-order word of the return value. The owner window
		// receives a WM_COMMAND message.
		execute = MNC_EXECUTE,

		// Informs the system that it should select the item specified in the low-order word of the return value.
		select = MNC_SELECT,
	};

	// Used for the WM_MENUCHAR message. Specifies the active menu type.
	enum class menuchar_type : std::uint16_t
	{
		// Indicates a drop-down menu, submenu, or shortcut menu.
		popup = MF_POPUP,

		// Indicates the window menu.
		window = MF_SYSMENU,
	};

	// Structure which translates the arguments of a WM_MENUCHAR message.
	// The WM_MENUCHAR message is sent went when a menu is active and the user presses a key that does not correspond to any
	// mnemonic or accelerator key.
	// View the MSDN documentation for the WM_MENUCHAR message.
	struct wm_menuchar
	{
		wm_menuchar() noexcept = default;

		wm_menuchar(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			key(static_cast<std::uint8_t>(LOWORD(Wparam))),
			menu_type(static_cast<menuchar_type>(HIWORD(Lparam))),
			hmenu(reinterpret_cast<HMENU>(Lparam))
		{
		}

		[[nodiscard]] static wm_menuchar init(std::uint8_t const Key, menuchar_type const MenuType, HMENU const Hmenu) noexcept
		{
			wm_menuchar v;
			v.key = Key;
			v.menu_type = MenuType;
			v.hmenu = Hmenu;
			return v;
		}

		// The character code that corresponds to the key the user pressed.
		std::uint8_t key;

		// The active menu type.
		menuchar_type menu_type;

		// A handle to the active menu.
		HMENU hmenu;

		// Makes a return value for a WM_MENUCHAR message.
		[[nodiscard]] static std::intptr_t make_return_code(menu_char_cmd const Cmd = menu_char_cmd::ignore, std::uint16_t const Item = 0) noexcept
		{
			return MAKELRESULT(Item, Cmd);
		}
	};

	// Structure which translates the arguments of a WM_SYSCHAR message.
	//
	// For undocumented fields, refer to the generic_key_args structure.
	//
	// View the MSDN documentation for the WM_SYSCHAR message.
	using wm_syschar = generic_system_key_args;

	// Possible menu flags received in the WM_MENUSELECT message. Get these flags from the high-order WORD of the WPARAM
	// argument.
	enum class menu_select_flags : std::uint16_t
	{
		none,
		grayed = MF_GRAYED, // Item is grayed.
		disabled = MF_DISABLED, // Item is disabled.
		bitmap = MF_BITMAP, // Item displays a bitmap.
		checked = MF_CHECKED, // Item is checked.
		popup = MF_POPUP, // Item opens a drop-down menu or submenu.
		highlighted = MF_HILITE, // Item is highlighted.
		owner_drawn = MF_OWNERDRAW, // Item is an owner-drawn item.
		window_menu = MF_SYSMENU, // Item is contained in the window menu.
		mouse_selected = MF_MOUSESELECT, // Item is selected with the mouse.
	};
	WDUL_DECLARE_ENUM_FLAGS(menu_select_flags);

	// Specifies the context for the WM_MENUSELECT message.
	enum class menu_select_context : std::uint8_t
	{
		// The selected item is a command item.
		command,

		// The selected item opens a drop-down menu or submenu.
		popup,

		// The system has closed the menu.
		closed,
	};

	// Structure which translates the arguments of a WM_MENUSELECT message.
	//
	// The WM_MENUSELECT message is sent to a menu's owner window when the user selects a menu item.
	//
	// View the MSDN documentation for the WM_MENUSELECT message.
	struct wm_menuselect
	{
		wm_menuselect() noexcept = default;

		wm_menuselect(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			hmenu(reinterpret_cast<HMENU>(Lparam)),
			id(LOWORD(Wparam)),
			flags(static_cast<menu_select_flags>(HIWORD(Wparam)))
		{
			if (flags == static_cast<menu_select_flags>(0xffff) && !hmenu)
			{
				context = menu_select_context::closed;
				return;
			}

			context = has_flag(flags, menu_select_flags::popup) ? menu_select_context::popup : menu_select_context::command;
		}

		[[nodiscard]] static wm_menuselect init(menu_select_context const Context, HMENU const Hmenu, std::uint16_t const Id, menu_select_flags const Flags) noexcept
		{
			wm_menuselect v;
			v.context = Context;
			v.hmenu = Hmenu;
			v.id = Id;
			v.flags = Flags;
			return v;
		}

		// Specifies the context of the WM_MENUSELECT message.
		menu_select_context context;

		// If the context field is menu_select_context::command, this field is a handle to the menu that was selected.
		// If the context field is menu_select_context::popup, this field is a handle to the main (selected) menu.
		// Use the GetSubMenu function to get the menu handle to the drop-down menu or submenu.
		// If the context field is menu_select_context::closed, this field is NULL.
		HMENU hmenu;

		// The menu item or submenu index.
		// If the context field is menu_select_context::command, this field specifies the identifier of the menu item.
		// If the context field is menu_select_context::popup, this field is the index of the drop-down menu or submenu in the
		// main menu.
		// If the context field is menu_select_context::closed, this field is indeterminate.
		std::uint16_t id;

		// Specifies one or more menu flags.
		// This field is always equal to 0xffff when the context field is menu_select_context::closed.
		menu_select_flags flags;
	};

	// Structure which translates the arguments of a WM_COMMAND message.
	// View the MSDN documentation for the WM_COMMAND message.
	struct wm_command
	{
		wm_command() noexcept = default;

		wm_command(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			id(LOWORD(Wparam)),
			code(HIWORD(Wparam)),
			hwnd(reinterpret_cast<HWND>(Lparam))
		{
		}

		[[nodiscard]] static wm_command init(std::uint16_t const Id, std::uint16_t const Code, HWND const Hwnd) noexcept
		{
			wm_command v;
			v.id = Id;
			v.code = Code;
			v.hwnd = Hwnd;
			return v;
		}

		// The menu identifier, accelerator identifier, or control identifier.
		std::uint16_t id;

		// When the source of the WM_COMMAND message is from a menu, this field is 0.
		// When the source of the WM_COMMAND message is from an accelerator, this field is 1.
		// For other message sources, this field is a control-defined notification code.
		std::uint16_t code;

		// Handle to the control window.
		// When the source of the WM_COMMAND message is from either a menu or an accelerator, this field is NULL.
		HWND hwnd;
	};

	// Structure which translates the arguments of a WM_MENUCOMMAND message.
	//
	// The WM_MENUCOMMAND message is sent when the user makes a selection from a menu, but is only sent for menus that
	// are defined with the MNS_NOTIFYBYPOS flag set in the dwStyle member of the MENUINFO structure.
	//
	// View the MSDN documentation for the WM_MENUCOMMAND message.
	struct wm_menucommand
	{
		wm_menucommand() noexcept = default;

		wm_menucommand(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			index(static_cast<std::uint32_t>(Wparam)),
			hmenu(reinterpret_cast<HMENU>(Lparam))
		{
		}

		[[nodiscard]] static wm_menucommand init(std::uint32_t const Index, HMENU const Hmenu) noexcept
		{
			wm_menucommand v;
			v.index = Index;
			v.hmenu = Hmenu;
			return v;
		}

		// The zero-based index of the item selected.
		std::uint32_t index;

		// A handle to the menu from which the item was selected.
		HMENU hmenu;
	};

	// Used for the SC_MONITORPOWER window menu command.
	enum class display_power : std::intptr_t
	{
		on = -1, // The display is powering on.
		low = 1, // The display is going to low power.
		off = 2, // The display is being shut off.
	};

	// Structure which translates the arguments of a WM_SYSCOMMAND message.
	// View the MSDN documentation for the WM_SYSCOMMAND message.
	struct wm_syscommand
	{
		wm_syscommand() noexcept = default;

		wm_syscommand(std::uintptr_t const Wparam, std::intptr_t const Lparam) noexcept :
			type(GET_SC_WPARAM(Wparam)),
			lparam(Lparam)
		{
		}

		[[nodiscard]] static wm_syscommand init(std::int32_t const Type, std::intptr_t const Lparam) noexcept
		{
			wm_syscommand v;
			v.type = Type;
			v.lparam = Lparam;
			return v;
		}

		// The type of window menu command requested.
		// E.g. SC_SIZE, SC_CLOSE, SC_MOVE, etc.
		std::int32_t type;

		// This field depends type field.
		// If a window menu command is chosen with the mouse, this is the position of the mouse in screen coordinates.
		std::intptr_t lparam;

		// Interprets the lparam field as a point in screen coordinates.
		// Use for commands which are chosen with the mouse, where the returned value is the position of the mouse in screen
		// coordinates.
		[[nodiscard]] vec2<std::int16_t> point() const noexcept
		{
			return { impl::get_x_lparam(lparam), impl::get_y_lparam(lparam) };
		}
	};
}
