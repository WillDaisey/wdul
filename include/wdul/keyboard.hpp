// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"

namespace wdul
{
	struct accelerator_table_handle_traits
	{
		using value_type = HACCEL;
		static value_type constexpr invalid_value = nullptr;

		static bool close(value_type const Value) noexcept
		{
			return ::DestroyAcceleratorTable(Value) != 0;
		}
	};

	using accelerator_table_handle = handle<accelerator_table_handle_traits>;

	// Flags used for the fVirt field of the ACCEL structure.
	// Specifies the accelerator behavior.
	enum class accelerator_flags : std::uint8_t
	{
		none,

		// The key field specifies a virtual-key code. If this flag is not specified, the key field is assumed to specify a
		// character code.
		virtual_key = FVIRTKEY,

		// No top-level menu item is highlighted when the accelerator is used. If this flag is not specified, a top-level menu
		// item will be highlighted, if possible, when the accelerator is used. This attribute is obsolete and retained only for
		// backward compatibility with resource files designed for 16-bit Windows.
		// no_invert = FNOINVERT,

		// The SHIFT key must be held down when the accelerator key is pressed.
		shift = FSHIFT,

		// The CTRL key must be held down when the accelerator key is pressed.
		control = FCONTROL,

		// The ALT key must be held down when the accelerator key is pressed.
		alt = FALT,
	};
	WDUL_DECLARE_ENUM_FLAGS(accelerator_flags);

	[[nodiscard]] inline ACCEL make_accelerator(std::uint16_t const Key, std::uint16_t const Cmd, accelerator_flags const Flags) noexcept
	{
		ACCEL accel{};
		accel.key = Key;
		accel.cmd = Cmd;
		accel.fVirt = to_underlying(Flags);
		return accel;
	}

	[[nodiscard]] inline accelerator_table_handle create_accelerator_table(std::int32_t const Count, _In_reads_(Count) ACCEL* const Entries)
	{
		return check_handle<accelerator_table_handle_traits>(CreateAcceleratorTableW(Entries, Count));
	}

	template <auto Count>
	[[nodiscard]] accelerator_table_handle create_accelerator_table(ACCEL(&Entries)[Count])
	{
		return check_handle<accelerator_table_handle_traits>(CreateAcceleratorTableW(Entries, Count));
	}

	inline std::int32_t copy_accelerator_table(_In_ HACCEL const AccelHandle, std::int32_t const Count, _Out_writes_to_opt_(Count, return) ACCEL* const Entries) noexcept
	{
		return CopyAcceleratorTableW(AccelHandle, Entries, Count);
	}

	// Returns the number of accelerator-table entries in the specified accelerator table.
	inline std::int32_t get_accelerator_table_size(_In_ HACCEL const AccelHandle) noexcept
	{
		return CopyAcceleratorTableW(AccelHandle, nullptr, 0);
	}
}
