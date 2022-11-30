// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"

namespace wdul
{
	/// <summary>Specifies whether a menu is a menu bar or a popup menu.</summary>
	enum class menu_type : bool
	{
		/// <summary>A horizontal menu bar, suitable for attaching to a top-level window.</summary>
		bar,

		/// <summary>A vertical popup menu, suitable for use as a submenu of another menu.</summary>
		popup,
	};

	/// <summary>Traits for the <c>generic_menu_handle</c> typedef, that is, a <c>wdul::handle</c> which manages a menu handle (<c>HMENU</c>).</summary>
	struct generic_menu_handle_traits
	{
		using value_type = HMENU;
		static value_type constexpr invalid_value = nullptr;

		static bool close(value_type const Value) noexcept
		{
			return DestroyMenu(Value) != 0;
		}
	};

	/// <summary>
	/// Manages a handle to a menu, using <c>DestroyMenu</c> to clean up resources associated with the menu.
	/// Note that if the menu is assigned to a window, resources are freed automatically by the window. You must detach the
	/// menu handle from the <c>generic_menu_handle</c> object when assigning it to a window.
	/// </summary>
	using generic_menu_handle = handle<generic_menu_handle_traits>;

	/// <summary>
	/// Equivalent to <c>generic_menu_handle_traits</c>, but provides a type-safe way of distinguishing popup menus from other menus.
	/// </summary>
	/// <typeparam name="MenuType">Specifies whether the traits are for a popup menu handle or a non-popup menu handle.</typeparam>
	template <menu_type MenuType>
	struct menu_handle_traits : generic_menu_handle_traits {};

	/// <summary>
	/// Traits for a <c>wdul::handle</c> which manages a menu handle to a menu which is not a popup menu.
	/// </summary>
	using menu_bar_handle = handle<menu_handle_traits<menu_type::bar>>;

	/// <summary>
	/// Traits for a <c>wdul::handle</c> which manages a menu handle to a popup menu.
	/// </summary>
	using menu_popup_handle = handle<menu_handle_traits<menu_type::popup>>;

	/// <summary>Flags which specify menu item state.</summary>
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

	/// <summary>Flags which specify the type of menu item.</summary>
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

	/// <summary>Creates an empty menu. You can use a <c>menu_item_builder</c> to insert items into the menu.</summary>
	/// <returns>A <c>menu_bar_handle</c> which manages a handle to the new menu.</returns>
	[[nodiscard]] inline menu_bar_handle create_menu()
	{
		return menu_bar_handle(check_pointer(CreateMenu()));
	}

	/// <summary>Creates an empty popup menu, that is, a drop-down menu, submenu, or shortcut menu.</summary>
	/// <returns>A <c>menu_popup_handle</c> which manages a handle to the new menu.</returns>
	[[nodiscard]] inline menu_popup_handle create_popup_menu()
	{
		return menu_popup_handle(check_pointer(CreatePopupMenu()));
	}

	/// <returns>The number of items in the menu specified by <paramref name="MenuHandle"/>.</returns>
	/// <param name="MenuHandle">A handle to the menu to get the item count of.</param>
	[[nodiscard]] inline std::uint16_t get_menu_item_count(_In_opt_ HMENU const MenuHandle)
	{
		std::int32_t const result = GetMenuItemCount(MenuHandle);
		if (result == -1) throw_last_error();
		return static_cast<std::uint16_t>(result);
	}

	/// <summary>Describes a menu item.</summary>
	class menu_item_builder
	{
	public:
		menu_item_builder() noexcept
		{
			mData.cbSize = sizeof(MENUITEMINFOW);
			mData.fMask = 0;
		}

		/// <summary>Sets the menu item identifier.</summary>
		/// <param name="Id">An application defined value which identifies the menu item.</param>
		void set_id(std::uint16_t const Id) noexcept
		{
			mData.fMask |= MIIM_ID;
			mData.wID = Id;
		}

		/// <summary>Specifies that the menu item is to be displayed using the given text string.</summary>
		/// <param name="Text">The string which specifies the text for the menu item to display.</param>
		void set_text(_In_z_ wchar_t const* const Text) noexcept
		{
			mData.fMask |= MIIM_STRING;
			mData.dwTypeData = const_cast<wchar_t*>(Text);
		}

		/// <summary>Sets the popup menu to be opened by the menu item.</summary>
		/// <param name="PopupMenuHandle">A handle to a popup menu (drop-down menu or submenu) to be associated with the menu item.</param>
		void set_popup(_In_ HMENU const PopupMenuHandle) noexcept
		{
			mData.fMask |= MIIM_SUBMENU;
			mData.hSubMenu = PopupMenuHandle;
		}

		/// <summary>Sets a bitmap to be displayed.</summary>
		/// <param name="BitmapHandle">A handle to the bitmap to be displayed, or a HBMMENU_* constant.</param>
		void set_bitmap(_In_ HBITMAP const BitmapHandle) noexcept
		{
			mData.fMask |= MIIM_BITMAP;
			mData.hbmpItem = BitmapHandle;
		}

		/// <summary>Sets the menu item type.</summary>
		/// <param name="TypeFlags">One or more flags from the <c>menu_item_type_flags</c> enumeration.</param>
		void set_type_flags(menu_item_type_flags const TypeFlags) noexcept
		{
			mData.fMask |= MIIM_FTYPE;
			mData.fType = to_underlying(TypeFlags);
		}

		/// <summary>Sets the menu item state.</summary>
		/// <param name="StateFlags">One or more flags from the <c>menu_item_state_flags</c> enumeration.</param>
		void set_state_flags(menu_item_state_flags const StateFlags) noexcept
		{
			mData.fMask |= MIIM_STATE;
			mData.fState = to_underlying(StateFlags);
		}

		/// <summary>
		/// Inserts a new menu item described by the <c>menu_item_builder</c> into the menu specified by <c>MenuHandle</c>.
		/// <para/>
		/// After the insertion takes place, the <c>menu_item_builder</c> is reset such that it no longer contains state which
		/// describes the menu item.
		/// </summary>
		/// <param name="MenuHandle">A handle to the menu to insert the menu item into.</param>
		/// <param name="Where">The identifier or position of the menu item before which to insert the new item. See <paramref name="InsertByPos"/>.</param>
		/// <param name="InsertByPos">If <c>true</c>, <paramref name="Where"/> is a position, otherwise, it is an identifier.</param>
		void insert(_In_ HMENU const MenuHandle, std::uint16_t const Where = 0xffff, bool const InsertByPos = true)
		{
			check_bool(InsertMenuItemW(MenuHandle, Where, InsertByPos, &mData));
			mData.fMask = 0;
		}

	private:
		MENUITEMINFOW mData;
	};
}
