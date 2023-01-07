// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "fs.hpp"

// Keep in mind that RIFF files store data in a little-endian format.
// Read more information about RIFF files: "https://docs.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-"

namespace wdul
{
	enum class riff_read_code : std::uint8_t
	{
		success,
		end,
		get_last_error,
	};

	enum class riff_reader_state : std::uint8_t
	{
		// The riff_reader is closed.
		closed,

		// The riff_reader is ready to read the chunk identifier and chunk length fields.
		chunk_info,

		// The riff_reader is ready to read the chunk data field.
		chunk_data,

		// The riff_reader is in an unknown state. Use the reposition method to restore the riff_reader to a known state.
		unknown,
	};

	enum class riff_reader_error_code : std::uint8_t
	{
		// No error occured.
		success,

		// The end of the file was reached.
		end,

		// The file was not found.
		not_found,

		// Access was denied.
		access_denied,

		// The RIFF file was incorrectly formatted.
		bad_format,
	};

	struct riff_chunk_info
	{
		// The chunk identifier.
		std::uint32_t id;

		// The length of the data field.
		std::uint32_t length;
	};

	class riff_reader
	{
	public:
		riff_reader(riff_reader const&) = delete;
		riff_reader& operator=(riff_reader const&) = delete;

		riff_reader() noexcept = default;

		riff_reader(riff_reader&& Other) noexcept :
			mFile(std::move(Other.mFile)),
			mFileType(Other.mFileType),
			mChunkInfo(Other.mChunkInfo),
			mState(std::exchange(Other.mState, riff_reader_state::closed))
		{
		}

		riff_reader& operator=(riff_reader Other) noexcept
		{
			swap(Other);
			return *this;
		}

		void swap(riff_reader& Other) noexcept
		{
			using std::swap;
			swap(mFile, Other.mFile);
			swap(mFileType, Other.mFileType);
			swap(mChunkInfo, Other.mChunkInfo);
			swap(mState, Other.mState);
		}

		[[nodiscard]] riff_reader_error_code open(_In_z_ wchar_t const* const Filename);
		void close() noexcept;


		// Reads the chunk identifier and chunk length fields from the current chunk. These values then can be retrieved from
		// the chunk_info method.
		//
		// If the chunk identifier and chunk length fields are read successfully, riff_reader_error_code::success is returned,
		// and the riff_reader_state is set to riff_reader_state::chunk_data.
		//
		// If the end of the file was reached, riff_reader_error_code::end is returned and state remains unchanged.
		//
		// If the RIFF file was incorrectly formatted, returns riff_reader_error_code::bad_format and the riff_reader_state is
		// set to riff_reader_state::unknown.
		//
		// If the riff_reader_state is not riff_reader_state::chunk_info, wrong_state is thrown.
		//
		// Provides a basic exception guarantee: If an exception is thrown, the riff_reader object is left in a valid state but
		// the riff_reader_state may have been set to riff_reader_state::unknown.
		[[nodiscard]] riff_reader_error_code read_chunk_info();


		// Reads the data field from the current chunk to the output buffer.
		//
		// The length of the data field can be retrieved using chunk_info().length. This value is the amount of bytes which will
		// be written to the output buffer.
		//
		// If the data field is read successfully, or the length of the data field is zero, returns
		// riff_reader_error_code::success, and the riff_reader_state is set to riff_reader_state::chunk_info.
		//
		// If the RIFF file was incorrectly formatted, returns riff_reader_error_code::bad_format and state remains unchanged.
		//
		// If the riff_reader_state is not riff_reader_state::chunk_data, wrong_state is thrown.
		//
		// Provides a basic exception guarantee: If an exception is thrown, the riff_reader object is left in a valid state but
		// the riff_reader_state may have been set to riff_reader_state::unknown.
		[[nodiscard]] riff_reader_error_code read_chunk_data(void* const Buffer);


		// Moves the file pointer to the beginning of the next chunk, excluding possible subchunks.
		// If the current chunk is the last, and the riff_reader_state is riff_reader_state::chunk_info,
		// riff_reader_error_code::end is returned.
		// If the riff_reader_state is riff_reader_state::chunk_data, riff_reader_error_code::success is the only possible return
		// value.
		//
		// If the function returns riff_reader_error_code::success, the riff_reader_state is set to riff_reader_state::chunk_info.
		//
		// If the riff_reader_state is not riff_reader_state::chunk_info or riff_reader_state::chunk_data, wrong_state is thrown.
		//
		// Provides a basic exception guarantee: If an exception is thrown, the riff_reader object is left in a valid state but
		// the riff_reader_state may have changed (however, the riff_reader_state will not be riff_reader_state::closed).
		[[nodiscard]] riff_reader_error_code seek_next_chunk();


		// Indicates to the riff_reader that it should read the subchunks found in the current chunk's data field.
		// In practical terms, the function transitions the riff_reader_state from riff_reader_state::chunk_data to
		// riff_reader_state::chunk_info.
		//
		// If the riff_reader_state is not riff_reader_state::chunk_info, wrong_state is thrown.
		// Provides a strong exception guarantee.
		void sink_to_subchunks();


		// Reads chunks, excluding subchunks of these chunks, until a chunk is found with the specified chunk identifier.
		// Chunks located before the current chunk, or the current chunk itself, will not be included in the search.
		//
		// If a chunk was found with the specified chunk identifier, riff_reader_error_code::success is returned and the
		// riff_reader_state is set to riff_reader_state::chunk_data.
		//
		// If the chunk wasn't found, riff_reader_error_code::end is returned.
		//
		// If the RIFF file was incorrectly formatted, riff_reader_error_code::bad_format is returned and the close() method is
		// called.
		// If the riff_reader_state is not riff_reader_state::closed, wrong_state is thrown.
		//
		// Provides a basic exception guarantee: If an exception is thrown, the riff_reader object is left in a valid state but
		// the riff_reader_state may have changed (however, the riff_reader_state will not be riff_reader_state::closed).
		[[nodiscard]] riff_reader_error_code find_chunk(std::uint32_t const ChunkId);


		// Sets the file pointer position, riff_chunk_info, and riff_reader_state.
		// If the riff_reader_state is riff_reader_state::closed, wrong_state is thrown.
		// Provides a strong exception guarantee.
		void reposition(std::int64_t const FilePtr, riff_chunk_info const& ChunkInfo, riff_reader_state const State);


		// Returns the current file pointer position.
		// If the riff_reader_state is riff_reader_state::closed, wrong_state is thrown.
		// Provides a strong exception guarantee.
		[[nodiscard]] std::int64_t file_pointer() const;


		// Sets the riff_reader_state value indicating the current state of the riff_reader object.
		// You can retrieve the current value by calling the state() method.
		void overwrite_state(riff_reader_state const State) noexcept { mState = State; }


		// Returns a FOURCC that identifies the file type.
		// This value is read when the riff_reader is opened (via the open method), and is uninitialised if the riff_reader has
		// not yet been opened. The field that this value is read from is the field before the data field in the RIFF chunk.
		[[nodiscard]] auto file_type() const noexcept { return mFileType; }


		// Returns the chunk identifier and chunk length last read.
		// The return value may be uninitialised if data hasn't yet been read.
		[[nodiscard]] auto const& chunk_info() const noexcept { return mChunkInfo; }


		// Returns the riff_reader_state value which indicates the current state of the riff_reader object.
		// View the riff_reader_state enum for more info.
		[[nodiscard]] auto state() const noexcept { return mState; }

	private:
		[[nodiscard]] riff_reader_error_code read_chunk_info_unchecked();
		static void skip_padding(_In_ HANDLE const File, std::uint32_t const ChunkLength);
		void skip_data_field_and_padding();

		file_handle mFile;
		std::uint32_t mFileType;
		riff_chunk_info mChunkInfo;
		riff_reader_state mState = riff_reader_state::closed;
	};

	inline void swap(riff_reader& Lhs, riff_reader& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}
}
