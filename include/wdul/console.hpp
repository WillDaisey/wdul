// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"
#include "error.hpp"

namespace wdul
{
	enum class std_handle_id : std::uint32_t
	{
		input = STD_INPUT_HANDLE,
		output = STD_OUTPUT_HANDLE,
		error = STD_ERROR_HANDLE,
	};

	[[nodiscard]] inline void* get_std_handle(std_handle_id const Id)
	{
		return check_handle(GetStdHandle(to_underlying(Id)));
	}

	// Wraps the AllocConsole and FreeConsole functions.
	class console_scope
	{
	public:
		console_scope(console_scope const&) = delete;
		console_scope(console_scope&&) = delete;
		console_scope& operator=(console_scope const&) = delete;
		console_scope& operator=(console_scope&&) = delete;

		console_scope()
		{
			check_bool(AllocConsole());
		}

		~console_scope()
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(FreeConsole(), == 0);
		}

	private:
	};

	enum class console_input_mode : std::uint32_t
	{
		none,
		enable_processed_input = 0x1,
		enable_line_input = 0x2,
		enable_echo_input = 0x4,
		enable_window_input = 0x8,
		enable_mouse_input = 0x10,
		enable_insert_mode = 0x20,
		enable_quick_edit_mode = 0x40,
		enable_extended_flags = 0x80,
		enable_auto_position = 0x100,
		enable_virtual_terminal_input = 0x200,
	};
	WDUL_DECLARE_ENUM_FLAGS(console_input_mode);

	enum class console_output_mode : std::uint32_t
	{
		none,
		enable_processed_output = 0x1,
		enable_wrap_at_eol_output = 0x2,
		enable_virtual_terminal_processing = 0x4,
		disable_newline_auto_return = 0x8,
		enable_lvb_grid_worldwide = 0x10,
	};
	WDUL_DECLARE_ENUM_FLAGS(console_output_mode);

	class console_input_handle
	{
	public:
		explicit console_input_handle(void* const ConsoleInputHandle) noexcept :
			mHandle(ConsoleInputHandle)
		{
		}

		std::uint32_t read(std::uint32_t const BufferLength, _Out_writes_(BufferLength) wchar_t* Buffer) const
		{
			DWORD charsRead;
			check_bool(ReadConsoleW(mHandle, Buffer, BufferLength, &charsRead, nullptr));
			return charsRead;
		}

		template <std::uint32_t BufferSize>
		std::uint32_t read(wchar_t(&Buffer)[BufferSize]) const
		{
			return read(BufferSize, Buffer);
		}

		void set_mode(console_input_mode const Mode) const
		{
			check_bool(SetConsoleMode(mHandle, to_underlying(Mode)));
		}

		[[nodiscard]] console_input_mode get_mode() const
		{
			DWORD mode;
			check_bool(GetConsoleMode(mHandle, &mode));
			return static_cast<console_input_mode>(mode);
		}

		[[nodiscard]] void* underlying() const noexcept
		{
			return mHandle;
		}

	private:
		void* mHandle;
	};

	class console_output_handle
	{
	public:
		explicit console_output_handle(void* const ConsoleOutputHandle) noexcept :
			mHandle(ConsoleOutputHandle)
		{
		}

		void write(std::wstring_view const String) const
		{
			check_bool(WriteConsoleW(mHandle, String.data(), static_cast<std::uint32_t>(String.size()), nullptr, nullptr));
		}

		void set_mode(console_output_mode const Mode) const
		{
			check_bool(SetConsoleMode(mHandle, to_underlying(Mode)));
		}

		[[nodiscard]] console_output_mode get_mode() const
		{
			DWORD mode;
			check_bool(GetConsoleMode(mHandle, &mode));
			return static_cast<console_output_mode>(mode);
		}

		[[nodiscard]] void* underlying() const noexcept
		{
			return mHandle;
		}

	private:
		void* mHandle;
	};
}
