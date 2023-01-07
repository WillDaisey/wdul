// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"
#include "memory.hpp"
#include "access_control.hpp"
#include <stdexcept>

namespace wdul
{
	class file_too_large : public std::runtime_error
	{
	public:
		file_too_large() :
			std::runtime_error("file too large")
		{
		}
	};

	struct find_file_handle_traits
	{
		using value_type = void*;
		static value_type constexpr invalid_value = INVALID_HANDLE_VALUE;

		static bool close(value_type const Value) noexcept
		{
			return ::FindClose(Value) != 0;
		}
	};

	// Manages a handle to a file, using CloseHandle to free associated resources.
	// Note that CloseHandle may perform other actions such as committing data to a disk.
	using file_handle = generic_handle<invalid_handle_type::invalid_handle_value>;

	// Manages a search handle, using FindClose to free associated resources.
	// Search handles are returned by functions such as the FindFirstFile function.
	using find_file_handle = handle<find_file_handle_traits>;

	// Specifies why a file could not be opened.
	enum class fopen_code : std::uint8_t
	{
		// The file was opened successfully.
		success,

		// The specified file could not be found.
		not_found,

		// Access was denied.
		access_denied,

		// Cannot access the file because it is being used or locked by another process.
		in_use,

		// Could not create the file as it already exists.
		already_exists,
	};

	// File object access rights.
	enum class file_access : std::uint32_t
	{
		none = 0x0,

		// For a file object, the right to read the corresponding file data.
		// For a directory object, the right to read the corresponding directory data and the right to list the contents of the directory.
		read_data = 0x1,

		// For a file object, the right to write data to the file. For a directory object, the right to create a file in the directory.
		write_data = 0x2,

		// For a file object, the right to append data to the file. (For local files, write operations will not overwrite existing
		// data if this flag is specified without file_access::write_data.)
		// For a directory object, the right to create a subdirectory.
		// For a named pipe, the right to create a pipe.
		append_data = 0x4,

		// The right to read extended file attributes.
		read_extended_attributes = 0x8,

		// The right to write extended file attributes.
		write_extended_attributes = 0x10,

		// For a native code file, the right to execute the file. This access right given to scripts may cause the script to be
		// executable, depending on the script interpreter.
		//
		// For a directory, the right to traverse the directory. By default, users are assigned the BYPASS_TRAVERSE_CHECKING
		// privilege, which ignores this access right.
		execute = 0x20,

		// For a directory, the right to delete a directory and all the files it contains, including read-only files.
		delete_child = 0x40,

		// The right to read file attributes.
		read_attributes = 0x80,

		// The right to write file attributes.
		write_attributes = 0x100,

		// All possible access rights for a file.
		all = FILE_ALL_ACCESS,

	};
	WDUL_DECLARE_ENUM_FLAGS(file_access);

	using file_access_mask = access_mask<file_access>;

	// Used in the Windows API CreateFile function.
	// The requested sharing mode of the file or device, which can be read, write, both, delete, all of these, or none 
	enum class file_share_mode : std::uint32_t
	{
		// Prevents subsequent open operations on a file or device if they request delete, read, or write access.
		none = 0x0,

		// Enables subsequent open operations on a file or device to request read access.
		// Otherwise, no process can open the file or device if it requests read access.
		// This flag must be specified if the file or device has read access.
		read = 0x1,

		// Enables subsequent open operations on a file or device to request write access.
		// Otherwise, no process can open the file or device if it requests write access.
		// This flag must be specified if the file or device has write access or has a file maping with write access.
		write = 0x2,

		// Enables subsequent open operations on a file or device to request delete access.
		// Otherwise, no process can open the file or device if it requests delete access.
		// This flag must be specified if the file or device has delete access.
		delete_and_rename = 0x4,
	};
	WDUL_DECLARE_ENUM_FLAGS(file_share_mode);

	// Used in the Windows API CreateFile function.
	// Specifies an action to take on a file or device that exists or does not exist.
	enum class file_open_mode : std::uint32_t
	{
		// Creates a new file, only if it does not already exist.
		// If the specified file exists, the function fails and the last-error code is set to ERROR_FILE_EXISTS.
		// If the specified file does not exist and is a valid path to a writable location, a new file is created.
		create_new = CREATE_NEW,

		// Creates a new file, always.
		// If the specified file exists and is writable, the function overwrites the file, the function succeeds, and last-error code is set to ERROR_ALREADY_EXISTS.
		// If the specified file does not exist and is a valid path, a new file is created, the function succeeds, and the last-error code is set to zero.
		create_always = CREATE_ALWAYS,

		// Opens a file or device, only if it exists.
		// If the specified file or device does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND.
		open_existing = OPEN_EXISTING,

		// Opens a file and truncates it so that its size is zero bytes, only if it exists.
		// If the specified file does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND.
		// The calling process must open the file with the generic_access::write bit set as part of the access mask.
		truncate_existing = TRUNCATE_EXISTING
	};

	// Creates or opens a file or I/O device.
	// This function wraps the CreateFileW function. For further reading, view the MSDN documentation for CreateFileW.
	[[nodiscard]] _Success_(return == fopen_code::success) fopen_code fopen(
		_Outptr_ HANDLE* const FileHandle,
		_In_z_ wchar_t const* const Filename,
		file_open_mode const CreationDisposition,
		std::uint32_t const FlagsAndAttributes,
		file_access_mask const Access,
		file_share_mode const ShareMode
	);

	// Creates or opens a file or I/O device. Throws on failure.
	// This function wraps the CreateFileW function. For further reading, view the MSDN documentation for CreateFileW.
	[[nodiscard]] file_handle fopen(
		_In_z_ wchar_t const* const Filename,
		file_open_mode const CreationDisposition,
		std::uint32_t const FlagsAndAttributes,
		file_access_mask const Access,
		file_share_mode const ShareMode
	);

	// Returns the current position of the file pointer for the given file.
	// This function wraps the SetFilePointerEx function. For further reading, view the MSDN documentation for SetFilePointerEx.
	[[nodiscard]] inline std::int64_t fgetpos(_In_ HANDLE const FileHandle)
	{
		LARGE_INTEGER fp;
		check_bool(SetFilePointerEx(FileHandle, {}, &fp, FILE_CURRENT));
		return fp.QuadPart;
	}

	// Sets the specified file's file pointer to the given position.
	// Returns the new position of the file pointer.
	// This function wraps the SetFilePointerEx function. For further reading, view the MSDN documentation for SetFilePointerEx.
	inline std::int64_t fsetpos(_In_ HANDLE const FileHandle, std::int64_t const NewPos)
	{
		LARGE_INTEGER fp;
		check_bool(SetFilePointerEx(FileHandle, { .QuadPart = NewPos }, &fp, FILE_BEGIN));
		return fp.QuadPart;
	}

	// Moves the file pointer of the specified file, where Offset is the number of bytes to move the file pointer forward by.
	// If Offset is negative, the file pointer is moved backwards.
	// This function wraps the SetFilePointerEx function. For further reading, view the MSDN documentation for SetFilePointerEx.
	inline std::int64_t fwalk(_In_ HANDLE const FileHandle, std::int64_t const Offset)
	{
		LARGE_INTEGER fp;
		check_bool(SetFilePointerEx(FileHandle, { .QuadPart = Offset }, &fp, FILE_CURRENT));
		return fp.QuadPart;
	}

	// Retrieves the size of the specified file.
	inline std::int64_t fgetsize(_In_ HANDLE const FileHandle)
	{
		LARGE_INTEGER sz;
		check_bool(GetFileSizeEx(FileHandle, &sz));
		return sz.QuadPart;
	}

	// Returns true if a directory with the given name exists.
	// Returns false if the directory with the specified name does not exist, access was denied, or some other error occured.
	inline bool directory_exists(_In_z_ wchar_t const* const Filename) noexcept
	{
		auto const attributes = GetFileAttributesW(Filename);
		return (attributes != INVALID_FILE_ATTRIBUTES) && (attributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	// Returns true if a file (not a directory) with the given name exists.
	// Returns false if the file with the specified name does not exist, access was denied, or some other error occured.
	inline bool fexists(_In_z_ wchar_t const* const Filename)
	{
		auto const attributes = GetFileAttributesW(Filename);
		return (attributes != INVALID_FILE_ATTRIBUTES) && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}

	inline std::uint32_t fread(_In_ HANDLE const FileHandle, std::uint32_t const BufferSize, _Out_writes_bytes_to_(BufferSize, return) void* Buffer)
	{
		DWORD bytesRead;
		check_bool(ReadFile(FileHandle, Buffer, BufferSize, &bytesRead, nullptr));
		return bytesRead;
	}

	inline std::uint32_t fwrite(_In_ HANDLE const FileHandle, std::uint32_t const BufferSize, _In_reads_bytes_(BufferSize) void const* const Buffer)
	{
		DWORD bytesWritten;
		check_bool(WriteFile(FileHandle, Buffer, BufferSize, &bytesWritten, nullptr));
		return bytesWritten;
	}

	// Reads characters from the specified file until the specified delimiter is found, or the end of the file is reached.
	// The delimiter is specified by the range [Delim, Delim + DelimSize).
	// The range [Buffer, Buffer + BufferSize) is used as a buffer for each time data is read from the file.
	//
	// Preconditions:
	// DelimSize must be greater than zero.
	// BufferSize must be greater than zero.
	//
	// Effects:
	// Returns the number of bytes the file pointer moved forward by.
	// The file pointer moved forward such that it is positioned after the delimiter string, or, failing that, at the end of
	// the file.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	//
	// Exception effects:
	// The file pointer may have moved.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	std::int64_t fread_delimited(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer
	);

	// Reads characters from the specified file to the specified string until the specified delimiter is found, or the end of
	// the file is reached.
	// The delimiter is specified by the range [Delim, Delim + DelimSize).
	// The range [Buffer, Buffer + BufferSize) is used as a buffer for each time data is read from the file.
	// 
	// Preconditions:
	// DelimSize must be greater than zero.
	// BufferSize must be greater than zero.
	//
	// Effects:
	// Returns the number of bytes the file pointer moved forward by.
	// The file pointer moved forward such that it is positioned after the delimiter string, or, failing that, at the end of
	// the file.
	// The characters read from the file, excluding the delimiter, is appended to the string specified by Output.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	//
	// Exception effects:
	// The file pointer may have moved.
	// The contents of the string referenced by Output is indeterminate.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	std::int64_t fread_delimited_consecutive(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		std::u8string& Output,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer
	);

	// Calls Output.clear(), followed by fread_delimited_consecutive.
	inline std::int64_t fread_delimited(
		_In_ HANDLE const FileHandle,
		_In_range_(> , 0) std::uint32_t const DelimSize,
		_In_reads_(DelimSize) char8_t const* const Delim,
		std::u8string& Output,
		_In_range_(> , 0) std::uint32_t const BufferSize,
		_In_reads_(BufferSize) std::uint8_t* const Buffer
	)
	{
		Output.clear();
		return fread_delimited_consecutive(FileHandle, DelimSize, Delim, Output, BufferSize, Buffer);
	}

	// Reads characters from the specified file until the a new line is found, or the end of the file is reached.
	// A new line is represented by a carriage return character followed my a line feed character (CR+LF).
	// The range [Buffer, Buffer + BufferSize) is used as a buffer for each time data is read from the file.
	// 
	// Preconditions:
	// BufferSize must be greater than 1.
	//
	// Effects:
	// Returns the number of bytes the file pointer moved forward by.
	// The file pointer will have moved forward such that it is positioned after the delimiter string, or,
	// failing that, at the end of the file.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	//
	// Exception effects:
	// The file pointer may have moved.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	inline std::int64_t freadline(_In_ HANDLE const FileHandle, std::uint32_t const BufferSize, std::uint8_t* const Buffer)
	{
		char8_t const delimiter[] = { u8'\r', u8'\n' };
		return fread_delimited(FileHandle, sizeof(delimiter), delimiter, BufferSize, Buffer);
	}

	// Reads characters from the specified file to the string specified by Output until a new line is found, or the end of the
	// file is reached. A new line is represented by a carriage return character followed my a line feed character (CR+LF).
	// The characters which make up the new line are not included in the output string, however, carriage return and line feed
	// characters may appear separately in the output string.
	// The range [Buffer, Buffer + BufferSize) is used as a buffer for each time data is read from the file.
	// 
	// Preconditions:
	// BufferSize must be greater than 1.
	//
	// Effects:
	// Returns the number of bytes the file pointer moved forward by.
	// The file pointer will have moved forward such that it is positioned after the delimiter string, or,
	// failing that, at the end of the file.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	//
	// Exception effects:
	// The file pointer may have moved.
	// The contents of the range [Buffer, Buffer + BufferSize) is indeterminate.
	inline std::int64_t freadline(_In_ HANDLE const FileHandle, std::u8string& Output, std::uint32_t const BufferSize, std::uint8_t* const Buffer)
	{
		char8_t const delimiter[] = { u8'\r', u8'\n' };
		return fread_delimited(FileHandle, sizeof(delimiter), delimiter, Output, BufferSize, Buffer);
	}

	/// <summary>Reads a file to an array of bytes.</summary>
	/// <param name="Filename">Pointer to a null-terminated UTF-16 string which contains the name of the file to be opened and read.</param>
	/// <returns>A <c>byte_array</c> containing the bytes read from the file.</returns>
	[[nodiscard]] byte_array read_bytes(_In_z_ wchar_t const* const Filename);
}
