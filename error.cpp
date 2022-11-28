// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/error.hpp"
#include "include/wdul/memory.hpp"
#include <stdexcept>
#include <ios>
#include <sstream>

namespace wdul
{
	// hresult_to_wcode converts custom FACILITY_ITF HRESULT codes into a zero-based value.
	// See https://docs.microsoft.com/en-us/windows/win32/com/codes-in-facility-itf.

	inline auto constexpr wcode_hresult_first = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0200);
	inline auto constexpr wcode_hresult_last = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0xffff);

	[[nodiscard]] inline WORD hresult_to_wcode(HRESULT const Hresult) noexcept
	{
		return (Hresult >= wcode_hresult_first && Hresult <= wcode_hresult_last) ? static_cast<WORD>(Hresult - wcode_hresult_first) : 0u;
	}

	[[noreturn]] void impl::hresult_to_exception(HRESULT const Hresult, std::string_view const Message)
	{
		switch (Hresult)
		{
		case E_ACCESSDENIED:
			throw hresult_access_denied(Message);

		case E_HANDLE:
			throw hresult_invalid_handle(Message);

		case E_INVALIDARG:
		{
			std::string msg = "Invalid argument";
			if (!Message.empty())
			{
				msg += ": ";
				msg += Message;
			}
			throw std::invalid_argument(msg.data());
		}

		case E_NOINTERFACE:
			throw hresult_no_interface(Message);

		case E_NOTIMPL:
			throw hresult_not_implemented(Message);

		case E_OUTOFMEMORY:
			throw std::bad_alloc();

		case E_POINTER:
			throw hresult_invalid_pointer(Message);

		case E_UNEXPECTED:
			throw hresult_unexpected_error(Message);

		case HRESULT_FROM_WIN32(ERROR_INVALID_STATE):
			throw hresult_invalid_state(Message);

		default:
			throw hresult_error(Hresult, Message);
		}
	}

	[[nodiscard]] std::string find_system_message(std::uint32_t const MessageId)
	{
		char* ptr;
		std::uint32_t len = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			MessageId,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<char*>(&ptr),
			0,
			nullptr
		);
		if (!ptr)
		{
			throw_last_error();
		}
		if (len > 1 && ptr[len - 1] == '\n')
		{
			ptr[--len] = '\0';
			if (ptr[len - 1] == '\r') ptr[--len] = '\0';
		}
		auto deallocator = finally_always([&]() { local_allocator::deallocate(ptr); });
		static_assert(static_cast<std::uintmax_t>((std::numeric_limits<std::uint32_t>::max)()) <= static_cast<std::uintmax_t>((std::numeric_limits<std::size_t>::max)()));
		std::string result(ptr, len);
		ptr = nullptr;
		return result;
	}

	[[nodiscard]] std::string find_win32_message(std::uint32_t const MessageId)
	{
		return find_system_message(MessageId);
	}

	[[nodiscard]] std::string find_hresult_message(HRESULT const Hresult)
	{
		try
		{
			return find_system_message(static_cast<std::uint32_t>(Hresult));
		}
		catch (std::exception const&)
		{
		}
		auto const wcode = hresult_to_wcode(Hresult);
		std::stringstream result;
		if (wcode == 0)
		{
			result << "Unknown error 0x" << std::hex << Hresult;
		}
		else
		{
			result << "Unknown error #" << wcode;
		}
		return std::move(result).str();
	}

	hresult_error::hresult_error(HRESULT const Hresult, std::string_view const Message)
	{
		std::stringstream ss;
		ss << "Error 0x" << std::hex << Hresult << " occured: " << find_hresult_message(Hresult);
		if (!Message.empty())
		{
			ss << "\n" << Message;
		}
		mData = new data(Hresult, std::move(ss).str());
	}

	hresult_error::hresult_error(hresult_error const& Other) noexcept :
		mData(Other.mData)
	{
		if (mData)
		{
			InterlockedIncrement(&mData->count);
		}
	}

	hresult_error::hresult_error(hresult_error&& Other) noexcept :
		mData(std::exchange(Other.mData, nullptr))
	{
	}

	hresult_error& hresult_error::operator=(hresult_error const& Other) noexcept
	{
		auto const p = Other.mData;
		if (p)
		{
			InterlockedIncrement(&p->count);
		}
		if (mData)
		{
			if (InterlockedDecrement(&mData->count) == 0)
			{
				delete mData;
			}
		}
		mData = p;
		return *this;
	}

	hresult_error& hresult_error::operator=(hresult_error&& Other) noexcept
	{
		auto temp = std::exchange(Other.mData, nullptr);
		if (mData)
		{
			if (InterlockedDecrement(&mData->count) == 0)
			{
				delete mData;
			}
		}
		mData = temp;
		return *this;
	}

	hresult_error::~hresult_error()
	{
		if (mData)
		{
			if (InterlockedDecrement(&mData->count) == 0)
			{
				delete mData;
			}
		}
	}
}
