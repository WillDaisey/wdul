// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include <type_traits>
#include <concepts>
#include <utility>
#include <string_view>
#include <string>
#include <exception>
#include <Windows.h>

#ifndef WDUL_DEBUG_RAISE_LAST_ERROR_WHEN
#ifdef _DEBUG
#define WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(Call, ErrorCond) if (Call ErrorCond) { ::RaiseException(::GetLastError(), 0, 0, nullptr); }
#else
#define WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(Call, ErrorCond) Call
#endif
#endif

namespace wdul
{
	// Searches the system message-table resource(s) for the message associated with the given Win32 error code.
	// Returns an ANSI string of the message.
	[[nodiscard]] std::string find_win32_message(std::uint32_t const MessageId);

	// Returns an ANSI-string error message which describes the given HRESULT.
	[[nodiscard]] std::string find_hresult_message(HRESULT const Hresult);

	class hresult_error : public std::exception
	{
	public:
		explicit hresult_error(HRESULT const Hresult, std::string_view const Message = {});

		hresult_error(hresult_error const& Other) noexcept;

		hresult_error(hresult_error&& Other) noexcept;

		hresult_error& operator=(hresult_error const& Other) noexcept;

		hresult_error& operator=(hresult_error&& Other) noexcept;

		~hresult_error();

		[[nodiscard]] char const* what() const noexcept override
		{
			return mData ? mData->str.data() : "";
		}

		[[nodiscard]] HRESULT error() const noexcept
		{
			return mData ? mData->error : E_FAIL;
		}

		void swap(hresult_error& Other) noexcept
		{
			std::swap(mData, Other.mData);
		}

	private:
		struct data
		{
			data(HRESULT const Error, std::string&& Str) :
				error(Error), str(std::move(Str))
			{
			}
			std::uint32_t count = 1;
			HRESULT error;
			std::string str;
		} *mData;
	};

	inline void swap(hresult_error& Lhs, hresult_error& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	class hresult_invalid_pointer : public hresult_error
	{
	public:
		explicit hresult_invalid_pointer(std::string_view const Message = {}) :
			hresult_error(E_POINTER, Message)
		{
		}
	};

	class hresult_invalid_handle : public hresult_error
	{
	public:
		explicit hresult_invalid_handle(std::string_view const Message = {}) :
			hresult_error(E_HANDLE, Message)
		{
		}
	};

	class hresult_no_interface : public hresult_error
	{
	public:
		explicit hresult_no_interface(std::string_view const Message = {}) :
			hresult_error(E_NOINTERFACE, Message)
		{
		}
	};

	class hresult_access_denied : public hresult_error
	{
	public:
		explicit hresult_access_denied(std::string_view const Message = {}) :
			hresult_error(E_ACCESSDENIED, Message)
		{
		}
	};

	class hresult_not_implemented : public hresult_error
	{
	public:
		explicit hresult_not_implemented(std::string_view const Message = {}) :
			hresult_error(E_NOTIMPL, Message)
		{
		}
	};

	class hresult_unexpected_error : public hresult_error
	{
	public:
		explicit hresult_unexpected_error(std::string_view const Message = {}) :
			hresult_error(E_UNEXPECTED, Message)
		{
		}
	};

	class hresult_invalid_state : public hresult_error
	{
	public:
		explicit hresult_invalid_state(std::string_view const Message = {}) :
			hresult_error(HRESULT_FROM_WIN32(ERROR_INVALID_STATE), Message)
		{
		}
	};

	namespace impl
	{
		[[noreturn]] void hresult_to_exception(HRESULT const Hresult, std::string_view const Message);
	}

	// Throws an exception based on the given HRESULT value specified by Hresult and the given optional message.
	// Possible exceptions thrown by this function are usually derived from hresult_error, but STL exceptions such as
	// std::bad_alloc may also be thrown. Regardless, all possible exceptions are derived from std::exception.
	[[noreturn]] inline void throw_hresult(HRESULT const Hresult, std::string_view const Message = {})
	{
		impl::hresult_to_exception(Hresult, Message);
	}

	// Calls throw_hresult with an HRESULT converted from the given Win32 error code.
	[[noreturn]] inline void throw_win32(std::uint32_t const ErrorCode, std::string_view const Message = {})
	{
		throw_hresult(HRESULT_FROM_WIN32(ErrorCode), Message);
	}

	// Calls throw_win32 with the calling thread's last-error code.
	[[noreturn]] inline void throw_last_error(std::string_view const Message = {})
	{
		throw_win32(GetLastError(), Message);
	}

	/// <summary>Calls <c>throw_hresult</c> with an <c>HRESULT</c> value converted from <paramref name="Nt"/>.</summary>
	/// <param name="Nt">The <c>NTSTATUS</c> which the exception will be based on.</param>
	/// <param name="Message">An optional message which describes what happened.</param>
	[[noreturn]] inline void throw_nt(NTSTATUS const Nt, std::string_view const Message = {})
	{
		throw_hresult(HRESULT_FROM_NT(Nt), Message);
	}

	// If the HRESULT value specified by Hresult indicates failure, calls throw_hresult with the given arguments.
	// Returns Hresult unchanged.
	_When_(Hresult < 0, _Analysis_noreturn_) inline HRESULT check_hresult(HRESULT const Hresult, std::string_view const Message = {})
	{
		if (FAILED(Hresult))
		{
			throw_hresult(Hresult, Message);
		}
		return Hresult;
	}

	// If the given Win32 error code specified by ErrorCode indicates failure, calls throw_win32 with the given arguments.
	// Returns ErrorCode unchanged.
	_When_(ErrorCode != 0, _Analysis_noreturn_) inline std::uint32_t check_win32(std::uint32_t const ErrorCode, std::string_view const Message = {})
	{
		if (ErrorCode != 0)
		{
			throw_win32(ErrorCode, Message);
		}
		return ErrorCode;
	}

	// If (!Result), calls throw_last_error with the given message.
	inline void check_bool(BOOL const Result, std::string_view const Message = {})
	{
		if (!Result)
		{
			throw_last_error(Message);
		}
	}

	// If the given pointer is nullptr, calls throw_last_error with the given message.
	// Returns the given pointer.
	template <class T>
	T* check_pointer(T* const Ptr, std::string_view const Message = {})
	{
		if (!Ptr)
		{
			throw_last_error(Message);
		}
		return Ptr;
	}

	// If the given pointer is INVALID_HANDLE_VALUE, calls throw_last_error with the given message.
	// Returns the given pointer.
	inline void* check_handle(void* const Ptr, std::string_view const Message = {})
	{
		if (Ptr == INVALID_HANDLE_VALUE)
		{
			throw_last_error(Message);
		}
		return Ptr;
	}

	/// <summary>If <c>Nt</c> is not equal to zero (STATUS_SUCCESS), throws an exception by calling <c>throw_nt</c>.</summary>
	/// <param name="Nt">The <c>NTSTATUS</c> which the exception will be based on.</param>
	/// <param name="Message">An optional message which describes what happened.</param>
	/// <returns><c>Nt</c>, unchanged.</returns>
	_When_(Nt != 0, _Analysis_noreturn_) inline NTSTATUS check_nt(NTSTATUS const Nt, std::string_view const Message = {})
	{
		if (Nt != 0)
		{
			throw_nt(Nt, Message);
		}
		return Nt;
	}
}
