// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"

namespace wdul
{
	// Indicates the hierarchical level of a menu.
	enum class menu_rank : std::uint8_t
	{
		// A horizontal menu bar, suitable for attaching to a top-level window.
		// The menu bar contains a list of drop-down menus.
		menu_bar,

		// Drop-down menus are contained in the menu bar.
		// Drop-down menus contain a list of submenus.
		dropdown_menu,

		// Submenus are contained in drop-down menus.
		// Submenus may contain further submenus.
		submenu,
	};

	// Specifies whether a menu is a menu bar or a popup menu.
	enum class menu_type : bool
	{
		// A horizontal menu bar, suitable for attaching to a top-level window.
		bar,

		// A vertical popup menu, suitable for use as a submenu of another menu.
		popup,
	};

	// Traits for a handle which manages a menu handle (HMENU).
	// See the generic_menu_handle typedef.
	struct generic_menu_handle_traits
	{
		using value_type = HMENU;
		static value_type constexpr invalid_value = nullptr;

		static bool close(value_type const Value) noexcept
		{
			return DestroyMenu(Value) != 0;
		}
	};

	// Manages a handle to a menu, using DestroyMenu to clean up resources associated with the menu.
	// Note that if the menu is assigned to a window, resources are freed automatically by the window. You must detach the
	// menu handle from the menu_handle object when assigning it to a window.
	using generic_menu_handle = handle<generic_menu_handle_traits>;

	// Equivalent to generic_menu_handle_traits, but provides a type-safe way of distinguishing popup menus from other menus.
	template <menu_type MenuType>
	struct menu_handle_traits : generic_menu_handle_traits {};

	// Similar to generic_menu_handle, but this type indicates the menu is not a popup menu.
	using menu_bar_handle = handle<menu_handle_traits<menu_type::bar>>;

	// Similar to generic_menu_handle, but this type indicates the menu is a popup menu.
	using menu_popup_handle = handle<menu_handle_traits<menu_type::popup>>;

	enum class menu_item_state_flags : std::uint16_t
	{
		none,

		// Disables the menu item and grays it so that it cannot be selected.
		disabled = MFS_DISABLED,

		// Checks the menu item.
		checked = MFS_CHECKED,

		// Highlights the menu item.
		highlighted = MFS_HILITE,

		// Specifies that the menu item is the default. A menu can contain only one default menu item.
		make_default = MFS_DEFAULT,

	};
	WDUL_DECLARE_ENUM_FLAGS(menu_item_state_flags);

	enum class menu_item_type_flags : std::uint16_t
	{
		none,

		// For a menu bar, places the item on a new line. For a popup menu, the item is placed in a new column which is separated
		// from the old column by a vertical line.
		menu_bar_break = MFT_MENUBARBREAK,

		// For a menu bar, places the item on a new line. For a popup menu, the item is placed in a new column which is not
		// separated from the old column by a vertical line.
		menu_break = MFT_MENUBREAK,

		// Assigns responsibility for drawing the menu item to the window that owns the menu. The window receives a WM_MEASUREITEM
		// message before the menu is displayed for the first time, and a WM_DRAWITEM message whenever the appearance of the menu
		// item must be updated. If this value is specified, the dwTypeData member contains an application-defined value.
		owner_drawn = MFT_OWNERDRAW,

		// Displays selected menu items using a radio-button mark instead of a check mark.
		radio_check = MFT_RADIOCHECK,

		// Right-justifies the menu item and any subsequent items. This value is valid only if the menu item is in a menu bar. 
		right_justify = MFT_RIGHTJUSTIFY,

		// Specifies that menus cascade right-to-left (the default is left-to-right). This is used to support right-to-left
		// languages, such as Arabic and Hebrew. 
		right_order = MFT_RIGHTORDER,
	};
	WDUL_DECLARE_ENUM_FLAGS(menu_item_type_flags);

	// Creates a menu using the CreateMenu function.
	[[nodiscard]] inline menu_bar_handle create_menu()
	{
		return menu_bar_handle(check_pointer(CreateMenu()));
	}

	// Creates a popup menu using the CreatePopupMenu function.
	[[nodiscard]] inline menu_popup_handle create_popup_menu()
	{
		return menu_popup_handle(check_pointer(CreatePopupMenu()));
	}

	// Returns the number of items in the given menu.
	[[nodiscard]] inline std::uint16_t get_menu_item_count(_In_opt_ HMENU const MenuHandle)
	{
		std::int32_t const result = GetMenuItemCount(MenuHandle);
		if (result == -1) throw_last_error();
		return static_cast<std::uint16_t>(result);
	}

	class menu_item_builder
	{
	public:
		menu_item_builder() noexcept
		{
			mData.cbSize = sizeof(MENUITEMINFOW);
			mData.fMask = 0;
		}

		void set_id(std::uint16_t const Id) noexcept
		{
			mData.fMask |= MIIM_ID;
			mData.wID = Id;
		}

		void set_text(_In_z_ wchar_t const* const Text) noexcept
		{
			mData.fMask |= MIIM_STRING;
			mData.dwTypeData = const_cast<wchar_t*>(Text);
		}

		void set_popup(_In_ HMENU const PopupMenuHandle) noexcept
		{
			mData.fMask |= MIIM_SUBMENU;
			mData.hSubMenu = PopupMenuHandle;
		}

		void set_bitmap(_In_ HBITMAP const BitmapHandle) noexcept
		{
			mData.fMask |= MIIM_BITMAP;
			mData.hbmpItem = BitmapHandle;
		}

		void set_type_flags(menu_item_type_flags const TypeFlags) noexcept
		{
			mData.fMask |= MIIM_FTYPE;
			mData.fType = to_underlying(TypeFlags);
		}

		void set_state_flags(menu_item_state_flags const StateFlags) noexcept
		{
			mData.fMask |= MIIM_STATE;
			mData.fState = to_underlying(StateFlags);
		}

		// Inserts a new menu item into the menu specified by MenuHandle.
		// The Where argument specifies the position or identifier of the menu item before which to insert the new item.
		// If the InsertByPos argument is true, the Where argument is a position, otherwise, it is an identifier.
		// These functions wrap the InsertMenuItemW function. For further reading, view the MSDN documentation for InsertMenuItemW.
		void insert(_In_ HMENU const MenuHandle, std::uint16_t const Where = 0xffff, bool const InsertByPos = true)
		{
			check_bool(InsertMenuItemW(MenuHandle, Where, InsertByPos, &mData));
			mData.fMask = 0;
		}

	private:
		MENUITEMINFOW mData;
	};
}
