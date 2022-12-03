// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "fs.hpp"

namespace wdul
{
	// For reading .ini files (https://en.wikipedia.org/wiki/INI_file).
	// The .ini file is expected to use UTF-8 encoding.
	class ini_file_reader
	{
	public:
		ini_file_reader(ini_file_reader const&) = delete;
		ini_file_reader& operator=(ini_file_reader const&) = delete;

		ini_file_reader() noexcept = default;
		ini_file_reader(ini_file_reader&&) noexcept = default;
		ini_file_reader& operator=(ini_file_reader&&) noexcept = default;

		// Opens the file specified by Filename, where Filename is a pointer to a null-terminated UTF-16 string.
		fopen_code open(_In_z_ wchar_t const* const Filename);

		void close() noexcept;

		// Searches the entire file for a section which has a name equivalent to the specified string.
		// If there are multiple sections with a matching name, the section closest to the start of the file will be found.
		// The current section is set to the found section, if any.
		// Returns true if and only if a matching section was found and therefore set.
		bool find_section(std::u8string_view const Section);

		// Searches the current section for a property (i.e. key-value pair) with the specified key.
		// If there are multiple properties with a matching key, the property closest to the start of the section will be found.
		// If a matching property is found, its value is written to the output string (specified by Value).
		// Returns true if and only if a matching property was found and therefore had its value written to the output string.
		bool find_value(std::u8string& Value, std::u8string_view const Key);

		/// <returns><c>true</c> if and only if the <c>ini_file_reader</c> is currently open.</returns>
		[[nodiscard]] bool is_open() const noexcept { return !!mFile; }

		/// <returns>The name of the current section.</returns>
		[[nodiscard]] std::u8string const& get_section() const noexcept { return mSection; }

	private:
		file_handle mFile;
		std::u8string mNode;
		std::u8string mSection;
		std::int64_t mSectionFp = 0;
		std::uint8_t mReadBuffer[128];
	};
}
