// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/resource_interchange_file.hpp"

namespace wdul
{
	[[nodiscard]] riff_reader_error_code riff_reader::open(_In_z_ wchar_t const* const Filename)
	{
		if (mState != riff_reader_state::closed)
		{
			throw hresult_invalid_state();
		}

		file_handle file(CreateFileW(Filename, GENERIC_READ, FILE_SHARE_READ, {}, OPEN_EXISTING, 0, {}));
		if (!file)
		{
			auto const lastError = GetLastError();
			switch (lastError)
			{
			case ERROR_FILE_NOT_FOUND:
				return riff_reader_error_code::not_found;

			case ERROR_ACCESS_DENIED:
				return riff_reader_error_code::access_denied;

			default:
				throw_win32(lastError);
			}
		}

		DWORD buffer, numBytesRead;
		if (!ReadFile(file.get(), &buffer, 4, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != 4) return riff_reader_error_code::bad_format;
		if (buffer != MAKEFOURCC('R', 'I', 'F', 'F')) return riff_reader_error_code::bad_format;

		// Read the file size field. We don't currently use this field.
		if (!ReadFile(file.get(), &buffer, 4, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != 4) return riff_reader_error_code::bad_format;

		if (!ReadFile(file.get(), &mFileType, 4, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != 4) return riff_reader_error_code::bad_format;

		mFile.put_ref() = file.detach();
		mState = riff_reader_state::chunk_info;
		return riff_reader_error_code::success;
	}

	void riff_reader::close() noexcept
	{
		WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(mFile.try_close(), == false);
		mState = riff_reader_state::closed;
	}

	[[nodiscard]] riff_reader_error_code riff_reader::read_chunk_info()
	{
		if (mState != riff_reader_state::chunk_info)
		{
			throw hresult_invalid_state();
		}
		return read_chunk_info_unchecked();
	}

	[[nodiscard]] riff_reader_error_code riff_reader::read_chunk_info_unchecked()
	{
		DWORD numBytesRead;
		if (!ReadFile(mFile.get(), &mChunkInfo.id, 4, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != 4) return riff_reader_error_code::end;

		auto unknownifier = finally([&]() { mState = riff_reader_state::unknown; });

		if (!ReadFile(mFile.get(), &mChunkInfo.length, 4, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != 4) return riff_reader_error_code::bad_format;

		unknownifier.revoke();

		mState = riff_reader_state::chunk_data;
		return riff_reader_error_code::success;
	}

	[[nodiscard]] riff_reader_error_code riff_reader::read_chunk_data(void* const Buffer)
	{
		if (mState != riff_reader_state::chunk_data)
		{
			throw hresult_invalid_state();
		}

		DWORD numBytesRead;
		if (!ReadFile(mFile.get(), Buffer, mChunkInfo.length, &numBytesRead, {})) throw_last_error();
		if (numBytesRead != mChunkInfo.length) return riff_reader_error_code::bad_format;

		auto unknownifier = finally([&]() { mState = riff_reader_state::unknown; });
		skip_padding(mFile.get(), mChunkInfo.length);
		unknownifier.revoke();

		mState = riff_reader_state::chunk_info;
		return riff_reader_error_code::success;
	}

	// warning C4061: enumerator 'x' in switch of enum 'y' is not explicitly handled by a case label
#pragma warning(push)
#pragma warning(disable:4061)
	[[nodiscard]] riff_reader_error_code riff_reader::seek_next_chunk()
	{
		switch (mState)
		{
		case riff_reader_state::chunk_info:
		{
			auto const code = read_chunk_info();
			if (code != riff_reader_error_code::success) return code;
			skip_data_field_and_padding();
			return riff_reader_error_code::success;
		}
		case riff_reader_state::chunk_data:
			skip_data_field_and_padding();
			return riff_reader_error_code::success;

		default:
			throw hresult_invalid_state();
		}
	}
#pragma warning(pop)

	void riff_reader::sink_to_subchunks()
	{
		if (mState != riff_reader_state::chunk_data)
		{
			throw hresult_invalid_state();
		}
		mState = riff_reader_state::chunk_info;
	}

	[[nodiscard]] riff_reader_error_code riff_reader::find_chunk(std::uint32_t const ChunkId)
	{
		auto const saveFp = file_pointer(); // also checks state
		auto const saveChunkInfo = mChunkInfo;
		auto const saveState = mState;

		if (mState == riff_reader_state::chunk_data)
		{
			skip_data_field_and_padding();
		}

		while (true)
		{
			auto const code = read_chunk_info_unchecked();
			if (code != riff_reader_error_code::success)
			{
				if (code == riff_reader_error_code::end)
				{
					reposition(saveFp, saveChunkInfo, saveState);
				}
				return code;
			}
			if (mChunkInfo.id == ChunkId)
			{
				return riff_reader_error_code::success;
			}
			skip_data_field_and_padding();
		}
	}

	void riff_reader::reposition(std::int64_t const FilePtr, riff_chunk_info const& ChunkInfo, riff_reader_state const State)
	{
		if (mState == riff_reader_state::closed) throw hresult_invalid_state();
		LARGE_INTEGER distance;
		distance.QuadPart = FilePtr;
		if (!SetFilePointerEx(mFile.get(), distance, {}, FILE_BEGIN))
		{
			throw_last_error();
		}
		mChunkInfo = ChunkInfo;
		mState = State;
	}

	[[nodiscard]] std::int64_t riff_reader::file_pointer() const
	{
		if (mState == riff_reader_state::closed) throw hresult_invalid_state();
		LARGE_INTEGER fp;
		if (!SetFilePointerEx(mFile.get(), {}, &fp, FILE_CURRENT)) throw_last_error();
		return fp.QuadPart;
	}

	void riff_reader::skip_padding(_In_ HANDLE const File, std::uint32_t const ChunkLength)
	{
		if ((ChunkLength % 2) == 1)
		{
			// If the chunk length is odd, then a pad byte is added to the end of the chunk data.
			LARGE_INTEGER distance;
			distance.QuadPart = 1;
			if (!SetFilePointerEx(File, distance, NULL, FILE_CURRENT)) throw_last_error();
		}
	}

	void riff_reader::skip_data_field_and_padding()
	{
		WDUL_ASSERT(mState == riff_reader_state::chunk_data);

		bool padding = false;
		if ((mChunkInfo.length % 2) == 1)
		{
			padding = true;
		}

		LARGE_INTEGER distance;
		distance.QuadPart = static_cast<std::int64_t>(mChunkInfo.length) + padding;
		if (!SetFilePointerEx(mFile.get(), distance, NULL, FILE_CURRENT)) throw_last_error();

		mState = riff_reader_state::chunk_info;
	}
}
