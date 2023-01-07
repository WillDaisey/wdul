// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "handle.hpp"
#include "access_control.hpp"

namespace wdul
{
	using event_handle = generic_handle<invalid_handle_type::null>;
	using thread_handle = generic_handle<invalid_handle_type::null>;
	using mutex_handle = generic_handle<invalid_handle_type::null>;

	class critical_section
	{
	public:
		critical_section(critical_section const&) = delete;
		critical_section(critical_section&&) = delete;
		critical_section& operator=(critical_section) = delete;

		critical_section() noexcept
		{
			InitializeCriticalSection(&mCs);
		}

		explicit critical_section(std::uint32_t const SpinCount, std::uint32_t const Flags = CRITICAL_SECTION_NO_DEBUG_INFO)
		{
			if (InitializeCriticalSectionEx(&mCs, SpinCount, Flags) == 0)
			{
				throw_last_error();
			}
		}

		~critical_section() noexcept
		{
			DeleteCriticalSection(&mCs);
		}

		void lock() noexcept
		{
			EnterCriticalSection(&mCs);
		}

		auto scoped_lock() noexcept
		{
			lock();
			return finally_always([&]() { unlock(); });
		}

		auto revocable_scoped_lock() noexcept
		{
			lock();
			return finally([&]() { unlock(); });
		}

		bool try_lock() noexcept
		{
			return TryEnterCriticalSection(&mCs) != 0;
		}

		void unlock() noexcept
		{
			LeaveCriticalSection(&mCs);
		}

	private:
		CRITICAL_SECTION mCs;
	};

	// Used for CreateEventEx.
	enum class event_create_flags : std::uint32_t
	{
		none = 0x0,
		manual_reset = 0x1, // CREATE_EVENT_MANUAL_RESET
		initial_set = 0x2, // CREATE_EVENT_INITIAL_SET
	};
	WDUL_DECLARE_ENUM_FLAGS(event_create_flags);

	/*
	// Event object access rights.
	enum class event_access : std::uint32_t
	{
		none = 0x0,

		// Standard access rights:
		delete_object = to_underlying(standard_access::delete_object),
		read_control = to_underlying(standard_access::read_control),
		synchronize = to_underlying(standard_access::synchronize),
		write_dac = to_underlying(standard_access::write_dac),
		write_owner = to_underlying(standard_access::write_owner),

		// All possible access rights for an event object. Use this right only if your application requires access beyond that granted by the standard
		// access rights and EVENT_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = EVENT_ALL_ACCESS,

		// Modify state access, which is required for the SetEvent, ResetEvent and PulseEvent functions.
		modify_state = EVENT_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(event_access);

	// Mutex object access rights.
	enum class mutex_access : std::uint32_t
	{
		none = 0x0,

		// Standard access rights:
		delete_object = to_underlying(standard_access::delete_object),
		read_control = to_underlying(standard_access::read_control),
		synchronize = to_underlying(standard_access::synchronize),
		write_dac = to_underlying(standard_access::write_dac),
		write_owner = to_underlying(standard_access::write_owner),

		// All possible access rights for a mutex object. Use this right only if your application requires access beyond that granted by the standard access rights.
		// Using this access right increases the possibility that your application must be run by an Administrator.
		all = MUTEX_ALL_ACCESS,

		// Reserved for future use.
		// modify_state = MUTEX_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(mutex_access);

	// Semaphore object access rights.
	enum class semaphore_access : std::uint32_t
	{
		none = 0x0,

		// Standard access rights:
		delete_object = to_underlying(standard_access::delete_object),
		read_control = to_underlying(standard_access::read_control),
		synchronize = to_underlying(standard_access::synchronize),
		write_dac = to_underlying(standard_access::write_dac),
		write_owner = to_underlying(standard_access::write_owner),

		// All possible access rights for a semaphore object. Use this right only if your application requires access beyond that granted by the standard access
		// rights and SEMAPHORE_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = SEMAPHORE_ALL_ACCESS,

		// Modify state access, which is required for the ReleaseSemaphore function.
		modify_state = SEMAPHORE_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(semaphore_access);

	// Waitable timer object access rights.
	enum class waitable_timer_access : std::uint32_t
	{
		none = 0x0,

		// Standard access rights:
		delete_object = to_underlying(standard_access::delete_object),
		read_control = to_underlying(standard_access::read_control),
		synchronize = to_underlying(standard_access::synchronize),
		write_dac = to_underlying(standard_access::write_dac),
		write_owner = to_underlying(standard_access::write_owner),

		// All possible access rights for a waitable timer object. Use this right only if your application requires access beyond that granted by the standard access
		// rights and TIMER_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = TIMER_ALL_ACCESS,

		// Modify state access, which is required for the SetWaitableTimer and CancelWaitableTimer functions.
		modify_state = TIMER_MODIFY_STATE,

		// Reserved for future use.
		// query_state = TIMER_QUERY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(waitable_timer_access);
	*/

	// Event object access rights.
	enum class event_access : std::uint32_t
	{
		none = 0x0,

		// All possible access rights for an event object. Use this right only if your application requires access beyond that granted by the standard
		// access rights and EVENT_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = EVENT_ALL_ACCESS,

		// Modify state access, which is required for the SetEvent, ResetEvent and PulseEvent functions.
		modify_state = EVENT_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(event_access);

	using event_access_mask = access_mask<event_access>;

	// Mutex object access rights.
	enum class mutex_access : std::uint32_t
	{
		none = 0x0,

		// All possible access rights for a mutex object. Use this right only if your application requires access beyond that granted by the standard access rights.
		// Using this access right increases the possibility that your application must be run by an Administrator.
		all = MUTEX_ALL_ACCESS,

		// Reserved for future use.
		// modify_state = MUTEX_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(mutex_access);

	using mutex_access_mask = access_mask<mutex_access>;

	// Semaphore object access rights.
	enum class semaphore_access : std::uint32_t
	{
		none = 0x0,

		// All possible access rights for a semaphore object. Use this right only if your application requires access beyond that granted by the standard access
		// rights and SEMAPHORE_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = SEMAPHORE_ALL_ACCESS,

		// Modify state access, which is required for the ReleaseSemaphore function.
		modify_state = SEMAPHORE_MODIFY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(semaphore_access);

	using semaphore_access_mask = access_mask<semaphore_access>;

	// Waitable timer object access rights.
	enum class waitable_timer_access : std::uint32_t
	{
		none = 0x0,

		// All possible access rights for a waitable timer object. Use this right only if your application requires access beyond that granted by the standard access
		// rights and TIMER_MODIFY_STATE. Using this access right increases the possibility that your application must be run by an Administrator.
		all = TIMER_ALL_ACCESS,

		// Modify state access, which is required for the SetWaitableTimer and CancelWaitableTimer functions.
		modify_state = TIMER_MODIFY_STATE,

		// Reserved for future use.
		// query_state = TIMER_QUERY_STATE,
	};
	WDUL_DECLARE_ENUM_FLAGS(waitable_timer_access);

	using waitable_timer_access_mask = access_mask<waitable_timer_access>;

	[[nodiscard]] inline event_handle create_event(event_access_mask const DesiredAccess, event_create_flags const Flags = event_create_flags::none,
		_In_opt_ SECURITY_ATTRIBUTES* const EventAttributes = nullptr, _In_opt_z_ wchar_t const* const Name = nullptr)
	{
		event_handle h(CreateEventExW(EventAttributes, Name, to_underlying(Flags), DesiredAccess.underlying()));
		if (!h)
		{
			throw_last_error();
		}
		return h;
	}

	inline std::uint32_t wait_event(_In_ HANDLE const EventHandle, std::uint32_t const MillisecondsUntilTimeout = INFINITE, bool const Alertable = false)
	{
		auto const result = WaitForSingleObjectEx(EventHandle, MillisecondsUntilTimeout, Alertable);
		if (result == WAIT_FAILED)
		{
			throw_last_error();
		}
		return result;
	}

	template <std::uint32_t EventCount>
	inline std::uint32_t wait_any_event(HANDLE(&Handles)[EventCount], std::uint32_t const MillisecondsUntilTimeout = INFINITE, bool const Alertable = false)
	{
		auto const result = WaitForMultipleObjectsEx(EventCount, Handles, false, MillisecondsUntilTimeout, Alertable);
		if (result == WAIT_FAILED)
		{
			throw_last_error();
		}
		return result;
	}

	template <std::uint32_t EventCount>
	inline std::uint32_t wait_all_events(HANDLE(&Handles)[EventCount], std::uint32_t const MillisecondsUntilTimeout = INFINITE, bool const Alertable = false)
	{
		auto const result = WaitForMultipleObjectsEx(EventCount, Handles, true, MillisecondsUntilTimeout, Alertable);
		if (result == WAIT_FAILED)
		{
			throw_last_error();
		}
		return result;
	}

	inline void signal_event(_In_ HANDLE const EventHandle)
	{
		if (SetEvent(EventHandle) == 0)
		{
			throw_last_error();
		}
	}

	typedef DWORD(__stdcall* thread_start_routine)(void*) noexcept;
	[[nodiscard]] inline thread_handle create_thread(_In_ thread_start_routine const Function, _In_opt_ void* const Param,
		std::uint32_t const StackSize = 0, std::uint32_t const CreationFlags = 0)
	{
		thread_handle h(CreateThread(nullptr, StackSize, Function, Param, CreationFlags, nullptr));
		if (!h)
		{
			throw_last_error();
		}
		return h;
	}

	inline void wait_thread(_In_ HANDLE const ThreadHandle, std::uint32_t const MillisecondsUntilTimeout = INFINITE)
	{
		if (WaitForSingleObjectEx(ThreadHandle, MillisecondsUntilTimeout, false) == WAIT_FAILED)
		{
			throw_last_error();
		}
	}

	inline std::uint32_t resume_thread(_In_ HANDLE const ThreadHandle)
	{
		std::uint32_t result = ResumeThread(ThreadHandle);
		if (result == static_cast<std::uint32_t>(-1))
		{
			throw_last_error();
		}
		return result;
	}

	[[nodiscard]] inline bool is_thread_running(_In_ HANDLE const ThreadHandle)
	{
		auto result = WaitForSingleObjectEx(ThreadHandle, 0, false);
		if (result == WAIT_FAILED)
		{
			throw_last_error();
		}
		return result != WAIT_TIMEOUT;
	}
}
