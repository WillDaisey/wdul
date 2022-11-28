// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace wdul
{
	// Converts a UTF-8 string to a UTF-16 string.
	//
	// Preconditions:
	// [Utf8, Utf8 + Size) is a (possibly empty) range of characters where no such character is a null character.
	//
	// Effects:
	// Returns a UTF-16 string converted from the given string.
	// If Size is less than zero, an exception is thrown.
	[[nodiscard]] std::wstring utf8_to_utf16(
		_In_range_(>= , 0) std::int32_t const Size,
		_In_reads_(Size) char8_t const* const Utf8
	);

	// Converts a null-terminated UTF-8 string to a UTF-16 string.
	//
	// Preconditions:
	// Utf8 is a pointer to a null-terminated UTF-8 string.
	//
	// Effects:
	// Returns a UTF-16 string converted from the given string.
	[[nodiscard]] std::wstring utf8_to_utf16(_In_z_ char8_t const* const Utf8);

	// Converts a UTF-16 string to a UTF-8 string.
	//
	// Preconditions:
	// [Utf16, Utf16 + Size) is a (possibly empty) range of characters where no such character is a null character.
	//
	// Effects:
	// Returns a UTF-8 string converted from the given string.
	// If Size is less than zero, an exception is thrown.
	[[nodiscard]] std::u8string utf16_to_utf8(
		_In_range_(>= , 0) std::int32_t const Size,
		_In_reads_(Size) wchar_t const* const Utf16
	);

	// Converts a UTF-16 string to a UTF-8 string.
	// Utf16 is a pointer to a null-terminated UTF-16 string.
	//
	// Preconditions:
	// Utf16 is a pointer to a null-terminated UTF-16 string.
	//
	// Effects:
	// Returns a UTF-16 string converted from the given string.
	[[nodiscard]] std::u8string utf16_to_utf8(_In_z_ wchar_t const* const Utf16);
}
