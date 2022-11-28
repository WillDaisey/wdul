// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"

namespace wdul
{
	// Indicates the range [first, last).
	template <class T>
	struct range
	{
		using element_type = T;

		// Pointer to the first element in the range.
		element_type* first;

		// Pointer to one after the last element in the range.
		element_type* last;

		// Initialises a range object from a fixed-length array.
		template <std::size_t Size>
		[[nodiscard]] static constexpr range<element_type> init(element_type const(&Array)[Size]) noexcept
		{
			return { .first = Array, .last = Array + Size };
		}
	};

	// Searches the buffer specified by the character sequence [Buffer.first, Buffer.last) for the delimiter specified by the
	// character sequence [Delim.first, Delim.last).
	//
	// Preconditions:
	// Delim must not be an empty range (i.e. Delim.first should not compare equal to Delim.last).
	// 
	// Effects:
	// If the delimiter was found in the buffer, returns a range of the string in the buffer which matches the delimiter.
	//
	// If a substring from the start of the delimiter was found at the end of the buffer, but the delimiter was not found in
	// its entirety, the range of characters in the buffer which match that of the delimiter are returned.
	//
	// Otherwise, an empty range is returned.
	range<char8_t const> find_delimiter(range<char8_t const> const Buffer, range<char8_t const> const Delim) noexcept;
}
