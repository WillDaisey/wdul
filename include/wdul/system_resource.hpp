// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"

namespace wdul
{
	struct hbitmap_traits
	{
		using value_type = HBITMAP;
		static std::uint8_t constexpr image = IMAGE_BITMAP;

		static void destroy(value_type const Handle) noexcept
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(DeleteObject(Handle), == 0);
		}
	};

	struct hicon_traits
	{
		using value_type = HICON;
		static std::uint8_t constexpr image = IMAGE_ICON;

		static void destroy(value_type const Handle) noexcept
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(DestroyIcon(Handle), == 0);
		}
	};

	struct hcursor_traits
	{
		using value_type = HCURSOR;
		static std::uint8_t constexpr image = IMAGE_CURSOR;

		static void destroy(value_type const Handle) noexcept
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(DestroyCursor(Handle), == 0);
		}
	};

	[[nodiscard]] inline void* load_image(
		_In_opt_ HINSTANCE const InstanceHandle,
		std::uint8_t const ImageType,
		_In_z_ wchar_t const* const Name,
		std::int32_t const Width,
		std::int32_t const Height,
		std::uint32_t const Flags
	)
	{
		auto h = LoadImageW(InstanceHandle, Name, ImageType, Width, Height, Flags);;
		if (!h) throw_last_error();
		return { h };
	}

	enum class oem_bitmap : std::uint16_t
	{
		close = 32754, // OBM_CLOSE
		uparrow = 32753, // OBM_UPARROW
		dnarrow = 32752, // OBM_DNARROW
		rgarrow = 32751, // OBM_RGARROW
		lfarrow = 32750, // OBM_LFARROW
		reduce = 32749, // OBM_REDUCE
		zoom = 32748, // OBM_ZOOM
		restore = 32747, // OBM_RESTORE
		reduced = 32746, // OBM_REDUCED
		zoomd = 32745, // OBM_ZOOMD
		restored = 32744, // OBM_RESTORED
		uparrowd = 32743, // OBM_UPARROWD
		dnarrowd = 32742, // OBM_DNARROWD
		rgarrowd = 32741, // OBM_RGARROWD
		lfarrowd = 32740, // OBM_LFARROWD
		mnarrow = 32739, // OBM_MNARROW
		combo = 32738, // OBM_COMBO
		uparrowi = 32737, // OBM_UPARROWI
		dnarrowi = 32736, // OBM_DNARROWI
		rgarrowi = 32735, // OBM_RGARROWI
		lfarrowi = 32734, // OBM_LFARROWI

		old_close = 32767, // OBM_OLD_CLOSE
		size = 32766, // OBM_SIZE
		old_uparrow = 32765, // OBM_OLD_UPARROW
		old_dnarrow = 32764, // OBM_OLD_DNARROW
		old_rgarrow = 32763, // OBM_OLD_RGARROW
		old_lfarrow = 32762, // OBM_OLD_LFARROW
		btsize = 32761, // OBM_BTSIZE
		check = 32760, // OBM_CHECK
		checkboxes = 32759, // OBM_CHECKBOXES
		btncorners = 32758, // OBM_BTNCORNERS
		old_reduce = 32757, // OBM_OLD_REDUCE
		old_zoom = 32756, // OBM_OLD_ZOOM
		old_restore = 32755, // OBM_OLD_RESTORE
	};

	enum class oem_icon : std::uint16_t
	{
		application = 32512, // IDI_APPLICATION
		question = 32514, // IDI_QUESTION
		winlogo = 32517, // IDI_WINLOGO
		shield = 32518, // IDI_SHIELD
		warning = 32515, // IDI_WARNING, IDI_EXCLAMATION
		error = 32513, // IDI_ERROR, IDI_HAND
		information = 32516, // IDI_INFORMATION, IDI_ASTERISK
	};

	enum class oem_cursor : std::uint16_t
	{
		hourglass_arrow = 32650, // IDC_APPSTARTING
		arrow = 32512, // IDC_ARROW
		crosshair = 32515, // IDC_CROSS
		hand = 32649, // IDC_HAND
		help = 32651, // IDC_HELP
		ibeam = 32513, // IDC_IBEAM
		no = 32648, // IDC_NO
		sizeall = 32646, // IDC_SIZEALL
		size_northeast_southwest = 32643, // IDC_SIZENESW
		size_north_south = 32645, // IDC_SIZENS
		size_northwest_southeast = 32642, // IDC_NWSE
		size_west_east = 32644, // IDC_SIZEWE
		vertical_arrow = 32516, // IDC_UPARROW
		hourglass = 32514, // IDC_WAIT
		pin = 32671, // IDC_PIN
		person = 32672, // IDC_PERSON
	};

	[[nodiscard]] inline HCURSOR load_oem_cursor(oem_cursor const CursorType)
	{
		auto const cursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(to_underlying(CursorType)));
		if (!cursor) throw_last_error();
		return cursor;
	}

	// Returns the default size of an icon, in pixels.
	[[nodiscard]] inline vec2i default_icon_size() noexcept
	{
		return { GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON) };
	}
	// Returns the recommended size of a small icon, in pixels. Small icons typically appear in window captions and in small icon view.
	[[nodiscard]] inline vec2i default_small_icon_size() noexcept
	{
		return { GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON) };
	}

	[[nodiscard]] inline HICON load_oem_icon(oem_icon IconType, const vec2i& Size = default_icon_size())
	{
		auto const icon = static_cast<HICON>(LoadImageW(
			nullptr,
			MAKEINTRESOURCEW(to_underlying(IconType)),
			IMAGE_ICON,
			Size.x,
			Size.y,
			LR_SHARED
		));
		if (!icon) throw_last_error();
		return icon;
	}

	[[nodiscard]] inline HBITMAP load_oem_bitmap(oem_bitmap BitmapType)
	{
		auto const bmp = static_cast<HBITMAP>(LoadImageW(
			nullptr,
			MAKEINTRESOURCEW(to_underlying(BitmapType)),
			IMAGE_BITMAP,
			0, // original size
			0,
			LR_SHARED
		));
		if (!bmp) throw_last_error();
		return bmp;
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_file(_In_z_ wchar_t const* const Filename)
	{
		return static_cast<T::value_type>(load_image(nullptr, T::image, Filename, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED));
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_file(_In_z_ wchar_t const* const Filename, vec2i const& Size)
	{
		return static_cast<T::value_type>(load_image(nullptr, T::image, Filename, Size.x, Size.y, LR_LOADFROMFILE | LR_SHARED));
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ResourceName)
	{
		return static_cast<T::value_type>(load_image(InstanceHandle, T::image, ResourceName, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, std::uint16_t const Resource)
	{
		return load_shared_image_from_resource<T>(InstanceHandle, MAKEINTRESOURCEW(Resource));
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ResourceName, vec2i const& Size)
	{
		return static_cast<T::value_type>(load_image(InstanceHandle, T::image, ResourceName, Size.x, Size.y, LR_SHARED));
	}

	template <class T>
	[[nodiscard]] T::value_type load_shared_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, std::uint16_t const Resource, vec2i const& Size)
	{
		return load_shared_image_from_resource<T>(InstanceHandle, MAKEINTRESOURCEW(Resource), Size);
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_file(_In_z_ wchar_t const* const FileName)
	{
		return handle<T>(static_cast<T::value_type>(load_image(nullptr, T::image, FileName, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE)));
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_file(_In_z_ wchar_t const* const FileName, vec2i const& Size)
	{
		return handle<T>(static_cast<T>(load_image(nullptr, T::image, FileName, Size.x, Size.y, LR_LOADFROMFILE)));
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ResourceName)
	{
		return handle<T>(static_cast<T::value_type>(load_image(InstanceHandle, T::image, ResourceName, 0, 0, LR_DEFAULTSIZE)));
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, std::uint16_t const Resource)
	{
		return load_image_from_resource<T>(InstanceHandle, MAKEINTRESOURCEW(Resource));
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, _In_z_ wchar_t const* const ResourceName, vec2i const& Size)
	{
		return handle<T>(static_cast<T::value_type>(load_image(InstanceHandle, T::image, ResourceName, Size.x, Size.y, 0)));
	}

	template <class T>
	[[nodiscard]] handle<T> load_image_from_resource(_In_opt_ HINSTANCE const InstanceHandle, std::uint16_t const Resource, vec2i const& Size)
	{
		return load_image_from_resource<T>(InstanceHandle, MAKEINTRESOURCEW(Resource), Size);
	}
}
