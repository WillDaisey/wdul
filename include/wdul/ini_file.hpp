// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "fs.hpp"

namespace wdul
{
	/// <summary>Reads UTF-8 .ini files (https://en.wikipedia.org/wiki/INI_file).</summary>
	class ini_file_reader
	{
	public:
		ini_file_reader(ini_file_reader const&) = delete;
		ini_file_reader& operator=(ini_file_reader const&) = delete;

		ini_file_reader() noexcept
		{
			leave_uninitialized(mReadBuffer);
		}

		ini_file_reader(ini_file_reader&&) noexcept = default;
		ini_file_reader& operator=(ini_file_reader&&) noexcept = default;

		/// <summary>
		/// Opens the file specified by <paramref name="Filename"/>.
		/// </summary>
		/// <param name="Filename">A pointer to a null-terminated UTF-16 string which contains the name of the file to open.</param>
		/// <returns>
		/// One of the following values:<para/>
		/// <c>fopen_code::success</c><para/>
		/// <c>fopen_code::not_found</c><para/>
		/// <c>fopen_code::access_denied</c><para/>
		/// <c>fopen_code::in_use</c>
		/// </returns>
		fopen_code open(_In_z_ wchar_t const* const Filename);

		/// <summary>Closes the file.</summary>
		void close() noexcept;

		/// <summary>
		/// Searches the file for the specified section. If the section is found, the current section is set to the found section,
		/// otherwise, the current section remains unchanged. If there are multiple sections in the file with the specified name,
		/// the current section will be set to that closest to the start of the file.
		/// </summary>
		/// <param name="Section">The name of the section to search for.</param>
		/// <returns><c>true</c> if and only if the section was found.</returns>
		bool find_section(std::u8string_view const Section);

		/// <summary>
		/// Searches the current section for a property (key-value pair) with the key specified by <paramref name="Key"/>.
		/// If a property with the specified key is found, its value is written to <paramref name="Value"/>.
		/// If there are multiple properties with the specified key, the property closest to the start of the current section
		/// will be found.
		/// </summary>
		/// <param name="Value">A reference to a string to assign the found value to.</param>
		/// <param name="Key">The key to search for in the current section.</param>
		/// <returns>
		/// <c>true</c> if and only if the key was found and therefore had its associated value written to<paramref name="Value"/>
		/// </returns>
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
