// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/ini_file.hpp"
#include <algorithm>

namespace wdul
{
	inline bool ini_is_whitespace(char8_t const Ch) noexcept
	{
		return Ch == u8' ' || Ch == u8'\t';
	}

	// Specifies the type of an INI file node.
	enum class ini_node_type : std::uint8_t
	{
		ignore, // The node either contained no non-whitespace characters, or is a comment.
		property, // The node is a key-value pair.
		section, // The node marks the beginning of a section.
		unknown, // Could not interpret the node.
	};

	struct ini_node_parse
	{
		// The node type.
		ini_node_type type;

		union
		{
			struct
			{
				char8_t const* key_first;
				char8_t const* key_end;
				char8_t const* value_first;
				char8_t const* value_end;
			} property;
			struct
			{
				char8_t const* name_first;
				char8_t const* name_end;
			} section;
		};
	};

	void ini_parse_node(_Out_ ini_node_parse* Result, std::u8string_view const Node)
	{
		// Get the first non-whitespace character.
		auto const firstCh = std::find_if_not(Node.begin(), Node.end(), ini_is_whitespace);
		if (firstCh == Node.end() || *firstCh == u8';')
		{
			// The node is empty, contains only whitespace, or is a comment.
			Result->type = ini_node_type::ignore;
			return;
		}

		if (*firstCh == u8'[')
		{
			// The first non-whitespace character is an opening square bracket.

			// Get the section declaration closing square bracket.
			auto const closingSquareBracket = std::find(firstCh + 1, Node.end(), u8']');
			if (closingSquareBracket != Node.end())
			{
				// The closing square bracket was found.

				// Verify that there is no additional non-whitespace characters past the closing square bracket.
				auto junk = std::find_if_not(closingSquareBracket + 1, Node.end(), ini_is_whitespace);
				if (junk == Node.end())
				{
					// The section node is syntactically correct.
					Result->type = ini_node_type::section;
					Result->section.name_first = std::to_address(firstCh + 1);
					Result->section.name_end = std::to_address(closingSquareBracket);
					return;
				}
				// Unexpected text was found after the section declaration.
				Result->type = ini_node_type::unknown;
				return;
			}

			// There was no closing square bracket found.
		}

		// The node is not a section, let's see if it's a property.

		auto const keyValueDelim = std::find(firstCh, Node.end(), u8'=');
		if (keyValueDelim == Node.end())
		{
			// The key-value delimiter (=) was not found.
			Result->type = ini_node_type::unknown;
			return;
		}
		// The key-value delimiter was found.

		// keyEnd will be at the end of the key string.
		// If there is whitespace immediately before the key-value delimiter, keyEnd needs to move back.
		auto keyEnd = keyValueDelim;
		do
		{
			if (--keyEnd == Node.begin())
			{
				// The key string is empty.
				Result->type = ini_node_type::unknown;
				return;
			}
		} while (ini_is_whitespace(*keyEnd));
		// keyEnd should be one character past the last character of the key string.
		++keyEnd;

		// valueFirst will be at the first character of the value string.
		// If there is whitespace immediately after the key-value delimiter, valueFirst needs to move forward.
		auto valueFirst = std::find_if_not(keyValueDelim + 1, Node.end(), ini_is_whitespace);

		// Note that valueFirst may be equal to Node.end(), in which case, the value string is empty.

		// The property node is syntactically correct.
		Result->type = ini_node_type::property;
		Result->property.key_first = std::to_address(firstCh);
		Result->property.key_end = std::to_address(keyEnd);
		Result->property.value_first = std::to_address(valueFirst);
		Result->property.value_end = std::to_address(Node.end());
		return;
	}

	fopen_code ini_file_reader::open(_In_z_ wchar_t const* const Filename)
	{
		return fopen(mFile.reput(), Filename, file_open_mode::open_existing, 0, generic_access::read, file_share_mode::read);
	}

	void ini_file_reader::close() noexcept
	{
		WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(mFile.try_close(), == false);
	}

	bool ini_file_reader::find_section(std::u8string_view const Section)
	{
		if (!is_open()) throw hresult_invalid_state();
		ini_node_parse parse;
		fsetpos(mFile.get(), 0);

		while (freadline(mFile.get(), mNode, sizeof(mReadBuffer), mReadBuffer))
		{
			ini_parse_node(&parse, mNode);
			if (parse.type == ini_node_type::section)
			{
				if (std::equal(parse.section.name_first, parse.section.name_end, Section.begin(), Section.end()))
				{
					mSection.assign(parse.section.name_first, parse.section.name_end);
					mSectionFp = fgetpos(mFile.get());
					return true;
				}
			}
		}

		return false;
	}

	bool ini_file_reader::find_value(std::u8string& Value, std::u8string_view const Key)
	{
		if (!is_open()) throw hresult_invalid_state();
		ini_node_parse parse;
		fsetpos(mFile.get(), mSectionFp);

		while (freadline(mFile.get(), mNode, sizeof(mReadBuffer), mReadBuffer))
		{
			ini_parse_node(&parse, mNode);
			if (parse.type == ini_node_type::property)
			{
				if (std::equal(parse.property.key_first, parse.property.key_end, Key.begin(), Key.end()))
				{
					Value.assign(parse.property.value_first, parse.property.value_end);
					return true;
				}
			}
			else if (parse.type == ini_node_type::section)
			{
				return false;
			}
		}

		return false;
	}
}
