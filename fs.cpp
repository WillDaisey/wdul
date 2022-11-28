// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/fs.hpp"
#include "include/wdul/parse.hpp"

namespace wdul
{
	template <class ToT, class FromT>
	ToT file_size_cast(FromT const Size)
	{
		if (std::cmp_greater(Size, (std::numeric_limits<ToT>::max)()))
		{
			throw file_too_large();
		}
		return static_cast<ToT>(Size);
	}

	_Ret_range_(!= , fopen_code::success) fopen_code to_fopen_code(std::uint32_t const Win32)
	{
		switch (Win32)
		{
		case ERROR_FILE_NOT_FOUND:
			return fopen_code::not_found;

		case ERROR_ACCESS_DENIED:
			return fopen_code::access_denied;

		default:
			throw_win32(Win32);
		}
	}

	[[nodiscard]] _Success_(return == fopen_code::success) fopen_code fopen(
		_Outptr_result_nullonfailure_ HANDLE* const FileHandle,
		_In_z_ wchar_t const* const Filename,
		file_open_mode const CreationDisposition,
		std::uint32_t const FlagsAndAttributes,
		file_access_mask const Access,
		file_share_mode const ShareMode
	)
	{
		*FileHandle = CreateFileW(Filename, Access.underlying(), to_underlying(ShareMode), nullptr, to_underlying(CreationDisposition), FlagsAndAttributes, nullptr);
		if (!(*FileHandle))
		{
			return to_fopen_code(GetLastError());
		}
		return fopen_code::success;
	}

	[[nodiscard]] file_handle fopen(
		_In_z_ wchar_t const* const Filename,
		file_open_mode const CreationDisposition,
		std::uint32_t const FlagsAndAttributes,
		file_access_mask const Access,
		file_share_mode const ShareMode
	)
	{
		return check_handle<file_handle::traits>(CreateFileW(Filename, Access.underlying(), to_underlying(ShareMode), nullptr, to_underlying(CreationDisposition), FlagsAndAttributes, nullptr));
	}

	// TODO: Consider putting this structure in public header.
	struct unused_parameter {};

	enum class fread_delimit_mode
	{
		// No output string.
		no_write,
		// Write to the output string, excluding the delimiter.
		exclusive,
		// Write to the output string, including the delimiter.
		inclusive,
	};

	template <fread_delimit_mode Mode>
	std::int64_t fread_delimitx(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer,
		[[maybe_unused]] std::conditional_t<Mode != fread_delimit_mode::no_write, std::u8string&, unused_parameter> Output
	)
	{
		WDUL_ASSERT(DelimSize != 0);
		WDUL_ASSERT(Delim != nullptr);
		WDUL_ASSERT(BufferSize > 0);
		WDUL_ASSERT(Buffer != nullptr);

		auto const startFp = fgetpos(FileHandle);

		std::uint32_t numBytesRead, lastMatchingCharCount = 0;
		range<char8_t const> matchingChars, readRange;

		while (true)
		{
			numBytesRead = fread(FileHandle, BufferSize, Buffer);
			readRange.first = reinterpret_cast<char8_t*>(Buffer);
			readRange.last = readRange.first + numBytesRead;

			matchingChars = find_delimiter(readRange, { .first = Delim + lastMatchingCharCount, .last = Delim + DelimSize });
			auto const matchingCharCount = static_cast<std::uint32_t>(matchingChars.last - matchingChars.first);

			if constexpr (Mode == fread_delimit_mode::exclusive)
			{
				if (matchingCharCount == 0)
				{
					Output.append(Delim, Delim + lastMatchingCharCount);
				}
				auto last = readRange.last;
				if (matchingChars.first != matchingChars.last)
				{
					last = matchingChars.first;
				}
				Output.append(readRange.first, last);
			}

			if constexpr (Mode == fread_delimit_mode::inclusive)
			{
				Output.append(readRange.first, readRange.last);
			}

			if (matchingCharCount + lastMatchingCharCount == DelimSize)
			{
				// Delimiter was found.
				// Because more than enough characters may have been read from the file, the file pointer may need to move back such that
				// it is situated after the delimiter.
				auto const steps = -(readRange.last - matchingChars.last);
				fwalk(FileHandle, steps);
				return fgetpos(FileHandle) - startFp;
			}

			if (matchingCharCount == 0)
			{
				if (numBytesRead == 0)
				{
					return fgetpos(FileHandle) - startFp;
				}
				lastMatchingCharCount = 0;
			}
			else
			{
				lastMatchingCharCount += matchingCharCount;
			}
		}
	}

	std::int64_t fread_delimited(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer
	)
	{
		return fread_delimitx<fread_delimit_mode::no_write>(FileHandle, DelimSize, Delim, BufferSize, Buffer, {});
	}

	std::int64_t fread_delimited_consecutive(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		std::u8string& Output,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer
	)
	{
		return fread_delimitx<fread_delimit_mode::exclusive>(FileHandle, DelimSize, Delim, BufferSize, Buffer, Output);
	}

	[[nodiscard]] byte_array read_bytes(_In_z_ wchar_t const* const Filename)
	{
		auto f = fopen(Filename, file_open_mode::open_existing, FILE_FLAG_SEQUENTIAL_SCAN, generic_access::read, file_share_mode::read);
		auto const size = file_size_cast<std::uint32_t>(fgetsize(f.get()));

		using allocator = byte_array::allocator;
		auto data = static_cast<std::uint8_t*>(allocator::allocate(size));

		auto dataDeleter = finally([&]() { allocator::deallocate_unchecked(data); });

		fread(f.get(), size, data);

		f.close();

		dataDeleter.revoke();

		return byte_array(size, data, take_ownership);
	}
}
