// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/strconv.hpp"
#include "include/wdul/error.hpp"
#include "include/wdul/debug.hpp"
#include <stdexcept>

namespace wdul::impl
{
#ifdef  _DEBUG
	template <class T>
	void warn_present_null_chars(T const* const First, T const* const Last, _In_opt_z_ char const* const Msg)
	{
		for (auto p = First; p != Last; ++p)
		{
			if (*p == u8'\0')
			{
				debug::output(debug::categories::get_facility(), debug::categories::strings, debug::severity::warn, __func__, Msg);
			}
		}
	}
#endif

}

namespace wdul
{
	[[nodiscard]] std::wstring utf8_to_utf16(
		_In_range_(>= , 0) std::int32_t const Size,
		_In_reads_(Size) char8_t const* const Utf8
	)
	{
		if (Size < 1)
		{
			if (Size < 0)
			{
				throw std::out_of_range("Size cannot be negative");
			}

			// Return an empty string if Size is zero.
			// MultiByteToWideChar does not accept zero.
			return {};
		}

#ifdef _DEBUG
		impl::warn_present_null_chars(Utf8, Utf8 + Size, "The string specified by [Utf8, Utf8 + Size) contains a null character. This may cause unexpected behaviour.");
#endif

		auto const utf8Chars = reinterpret_cast<char const*>(Utf8);

		// utf16ChCount does not include a null terminator.
		auto const utf16ChCount = MultiByteToWideChar(CP_UTF8, 0, utf8Chars, Size, nullptr, 0);
		if (utf16ChCount == 0)
		{
			throw_last_error();
		}

		if constexpr (static_cast<std::make_unsigned_t<decltype(utf16ChCount)>>((std::numeric_limits<decltype(utf16ChCount)>::max)()) > (std::numeric_limits<std::size_t>::max)())
		{
			if (static_cast<std::make_unsigned_t<decltype(utf16ChCount)>>(utf16ChCount) > (std::numeric_limits<std::size_t>::max)())
			{
				throw std::length_error("string too long");
			}
		}

		std::wstring utf16String(static_cast<std::size_t>(utf16ChCount), WDUL_DEBUG_SWITCH(L'?', 0) /*initialisation is not required*/);
		if (MultiByteToWideChar(CP_UTF8, 0, utf8Chars, Size, utf16String.data(), utf16ChCount) == 0)
		{
			throw_last_error();
		}
		return utf16String;
	}

	[[nodiscard]] std::wstring utf8_to_utf16(_In_z_ char8_t const* const Utf8)
	{
		auto const utf8Chars = reinterpret_cast<char const*>(Utf8);

		// utf16StrLen includes a null terminator.
		auto const utf16StrLen = MultiByteToWideChar(CP_UTF8, 0, utf8Chars, -1, nullptr, 0);
		if (utf16StrLen == 0)
		{
			throw_last_error();
		}

		if constexpr (static_cast<std::make_unsigned_t<decltype(utf16StrLen)>>((std::numeric_limits<decltype(utf16StrLen)>::max)()) > (std::numeric_limits<std::size_t>::max)())
		{
			if (static_cast<std::make_unsigned_t<decltype(utf16StrLen)>>(utf16StrLen) > (std::numeric_limits<std::size_t>::max)())
			{
				throw std::length_error("string too long");
			}
		}

		std::wstring utf16String(static_cast<std::size_t>(utf16StrLen - 1 /*-1 for null term*/), WDUL_DEBUG_SWITCH(L'?', 0) /*initialisation is not required*/);

		// Overwriting the null terminator with another NULL character is ok:
		// "Modifying the past-the-end null terminator stored at data()+size() to any value other than CharT() has undefined behavior."
		if (MultiByteToWideChar(CP_UTF8, 0, utf8Chars, -1, utf16String.data(), utf16StrLen) == 0)
		{
			throw_last_error();
		}

		/* Old Method:
		std::wstring utf16(static_cast<std::size_t>(utf16StrLen), WDUL_DEBUG_SWITCH(L'?', 0));

		// A null character will be written at the end of utf16, this needs to be erased.
		if (MultiByteToWideChar(CP_UTF8, 0, utf8Chars, -1, utf16.data(), utf16StrLen) == 0)
		{
			throw_last_error();
		}

		// Erase the null terminator.
		utf16.pop_back();
		*/

		return utf16String;
	}

	[[nodiscard]] std::u8string utf16_to_utf8(
		_In_range_(>= , 0) std::int32_t const Size,
		_In_reads_(Size) wchar_t const* const Utf16
	)
	{
		if (Size < 1)
		{
			if (Size < 0)
			{
				throw std::out_of_range("Size cannot be negative");
			}

			// Return an empty string if Size is zero.
			// MultiByteToWideChar does not accept zero.
			return {};
		}

#ifdef _DEBUG
		impl::warn_present_null_chars(Utf16, Utf16 + Size, "The string specified by [Utf16, Utf16 + Size) contains a null character. This may cause unexpected behaviour.");
#endif

		// utf8ChCount does not include a null terminator.
		auto const utf8ChCount = WideCharToMultiByte(CP_UTF8, 0, Utf16, Size, nullptr, 0, nullptr, nullptr);
		if (utf8ChCount == 0)
		{
			throw_last_error();
		}

		if constexpr (static_cast<std::make_unsigned_t<decltype(utf8ChCount)>>((std::numeric_limits<decltype(utf8ChCount)>::max)()) > (std::numeric_limits<std::size_t>::max)())
		{
			if (static_cast<std::make_unsigned_t<decltype(utf8ChCount)>>(utf8ChCount) > (std::numeric_limits<std::size_t>::max)())
			{
				throw std::length_error("string too long");
			}
		}

		std::u8string utf8String(static_cast<std::size_t>(utf8ChCount), WDUL_DEBUG_SWITCH(L'?', 0) /*initialisation is not required*/);
		if (WideCharToMultiByte(CP_UTF8, 0, Utf16, Size, reinterpret_cast<char*>(utf8String.data()), utf8ChCount, nullptr, nullptr) == 0)
		{
			throw_last_error();
		}

		return utf8String;
	}

	[[nodiscard]] std::u8string utf16_to_utf8(_In_z_ wchar_t const* const Utf16)
	{
		// utf8StrLen includes the null terminator.
		auto const utf8StrLen = WideCharToMultiByte(CP_UTF8, 0, Utf16, -1, nullptr, 0, nullptr, nullptr);
		if (utf8StrLen == 0)
		{
			throw_last_error();
		}

		if constexpr (static_cast<std::make_unsigned_t<decltype(utf8StrLen)>>((std::numeric_limits<decltype(utf8StrLen)>::max)()) > (std::numeric_limits<std::size_t>::max)())
		{
			if (static_cast<std::make_unsigned_t<decltype(utf8StrLen)>>(utf8StrLen) > (std::numeric_limits<std::size_t>::max)())
			{
				throw std::length_error("string too long");
			}
		}

		std::u8string utf8String(static_cast<std::size_t>(utf8StrLen - 1 /*-1 for null term*/), WDUL_DEBUG_SWITCH(L'?', 0) /*initialisation is not required*/);

		// Overwriting the null terminator with another NULL character is ok:
		// "Modifying the past-the-end null terminator stored at data()+size() to any value other than CharT() has undefined behavior."
		if (WideCharToMultiByte(CP_UTF8, 0, Utf16, -1, reinterpret_cast<char*>(utf8String.data()), utf8StrLen, nullptr, nullptr) == 0)
		{
			throw_last_error();
		}

		return utf8String;
	}
}
