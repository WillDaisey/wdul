// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/parse.hpp"
#include "include/wdul/foundation.hpp"

namespace wdul
{
	range<char8_t const> find_delimiter(range<char8_t const> const Buffer, range<char8_t const> const Delim) noexcept
	{
		WDUL_ASSERT(Buffer.first <= Buffer.last);
		WDUL_ASSERT(Delim.first < Delim.last);

		auto const delimSize = Delim.last - Delim.first;

		/* An empty delimiter is not currently allowed.
			if (delimSize == 0)
			{
				// The delimiter was empty.
				return { Buffer.first, Buffer.first };
			}
		*/

		range<char8_t const> match{};

		for (auto p = Buffer.first; p != Buffer.last; ++p)
		{
			if (*p == Delim.first[match.last - match.first])
			{
				if (*p == *Delim.first)
				{
					// The first character of the delimiter was found.
					match.first = p;
					match.last = p + 1;
				}
				else
				{
					// The next character of the delimiter was found.
					++match.last;
				}
				if (match.last - match.first == delimSize)
				{
					// The delimiter was found in its entirety.
					return match;
				}
			}
			else
			{
				// Character does not match delimiter.
				match.first = p;
				match.last = p;
			}
		}

		// The delimiter was not found in its entirety.
		return match;
	}
}
