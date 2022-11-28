// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"
#include "error.hpp"
#include <bit>
#include <Windows.h>

namespace wdul
{
	namespace impl
	{
		template <class T = wchar_t>
			requires (std::same_as<T, char> || std::same_as<T, wchar_t>)
		[[nodiscard]] auto make_int_atom(std::uint16_t const Atom) noexcept
		{
			return reinterpret_cast<T*>(static_cast<std::uintptr_t>(Atom));
		}
	}

	// Manages a window class.
	class window_class
	{
		window_class(window_class const&) = delete;
		window_class& operator=(window_class const&) = delete;
	public:
		explicit window_class(HINSTANCE const InstanceHandle = nullptr, std::uint16_t const Atom = 0) noexcept :
			mInstanceHandle(InstanceHandle), mAtom(Atom)
		{
			WDUL_ASSERT_MSG(InstanceHandle != nullptr || Atom == 0, "Atom must be zero when InstanceHandle is nullptr");
		}

		window_class(window_class&& Other) noexcept :
			mInstanceHandle(Other.mInstanceHandle), mAtom(std::exchange(Other.mAtom, {}))
		{
		}

		window_class& operator=(window_class&& Other) noexcept
		{
			swap(Other);
			return *this;
		}

		~window_class()
		{
			unregister();
		}

		void initialize(HINSTANCE const InstanceHandle) noexcept
		{
			WDUL_ASSERT(mAtom == 0);
			mInstanceHandle = InstanceHandle;
		}

		void attach(std::uint16_t const WindowClassAtom) noexcept
		{
			WDUL_ASSERT(mInstanceHandle != nullptr);
			if (mAtom != WindowClassAtom)
			{
				unregister();
				mAtom = WindowClassAtom;
			}
		}

		void unregister() noexcept
		{
			if (mAtom != 0)
			{
				WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(UnregisterClassW(impl::make_int_atom(mAtom), mInstanceHandle), == 0);
			}
		}

		[[nodiscard]] auto get_hinstance() const noexcept
		{
			return mInstanceHandle;
		}

		[[nodiscard]] auto get_atom() const noexcept
		{
			return mAtom;
		}

		explicit operator bool() const noexcept
		{
			return mAtom != 0;
		}

		void swap(window_class& Other) noexcept
		{
			using std::swap;
			swap(mInstanceHandle, Other.mInstanceHandle);
			swap(mAtom, Other.mAtom);
		}

	private:
		HINSTANCE mInstanceHandle;
		std::uint16_t mAtom;
	};

	inline void swap(window_class& Lhs, window_class& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	// Wraps the BeginPaint and EndPaint functions.
	class window_paint_scope
	{
		window_paint_scope(window_paint_scope const&) = delete;
		window_paint_scope(window_paint_scope&&) = delete;
		window_paint_scope& operator=(window_paint_scope const&) = delete;
		window_paint_scope& operator=(window_paint_scope&&) = delete;
	public:
		explicit window_paint_scope(_In_ HWND const Hwnd) noexcept : mHwnd(Hwnd)
		{
			BeginPaint(mHwnd, &mPs);
		}

		~window_paint_scope()
		{
			EndPaint(mHwnd, &mPs);
		}

		[[nodiscard]] auto get_hwnd() const noexcept
		{
			return mHwnd;
		}

		[[nodiscard]] auto& get_ps() noexcept
		{
			return mPs;
		}

		[[nodiscard]] auto const& get_ps() const noexcept
		{
			return mPs;
		}

	private:
		PAINTSTRUCT mPs;
		HWND mHwnd;
	};

	enum class window_data_offset
	{
		procedure = GWLP_WNDPROC,
		instance = GWLP_HINSTANCE,
		parent = GWLP_HWNDPARENT,
		style = GWL_STYLE,
		exstyle = GWL_EXSTYLE,
		userdata = GWLP_USERDATA,
		id = GWLP_ID,
	};

	inline std::intptr_t set_window_data(_In_ HWND const Hwnd, window_data_offset const Idx, std::intptr_t const Val)
	{
		SetLastError(0);
		auto const result = SetWindowLongPtrW(Hwnd, to_underlying(Idx), Val);
		std::uint32_t code;
		if (result == 0 && (code = GetLastError()) != 0)
		{
			throw_win32(code);
		}
		return result;
	}

	inline std::uint32_t try_set_window_data(_In_ HWND const Hwnd, window_data_offset const Idx, std::intptr_t const Val) noexcept
	{
		SetLastError(0);
		std::uint32_t code;
		if (SetWindowLongPtrW(Hwnd, to_underlying(Idx), Val) == 0 && (code = GetLastError()) != 0)
		{
			return code;
		}
		return 0;
	}

	[[nodiscard]] inline std::intptr_t get_window_data(_In_ HWND const Hwnd, window_data_offset const Idx)
	{
		SetLastError(0);
		auto const value = GetWindowLongPtrW(Hwnd, to_underlying(Idx));
		std::uint32_t code;
		if (value == 0 && (code = GetLastError()) != 0)
		{
			throw_win32(code);
		}
		return value;
	}

	[[nodiscard]] inline std::uint32_t try_get_window_data(_In_ HWND const Hwnd, window_data_offset const Idx, std::intptr_t& Data) noexcept
	{
		SetLastError(0);
		Data = GetWindowLongPtrW(Hwnd, to_underlying(Idx));
		std::uint32_t code;
		if (Data == 0 && (code = GetLastError()) != 0)
		{
			return code;
		}
		return 0;
	}

	[[nodiscard]] std::intptr_t get_window_data_or_terminate(_In_ HWND const Hwnd, window_data_offset const Idx) noexcept;

	inline bool get_window_message(
		_Out_ MSG* const MessageData,
		_In_opt_ HWND const WindowHandle = nullptr,
		std::uint32_t const MinFilter = 0,
		std::uint32_t const MaxFilter = 0
	)
	{
		auto const res = GetMessageW(MessageData, WindowHandle, MinFilter, MaxFilter);
		if (res == -1) throw_last_error();
		return res != 0;
	}

	inline bool peek_window_message(
		_Out_ MSG* const MessageData,
		_In_opt_ HWND const WindowHandle = nullptr,
		std::uint32_t const MinFilter = 0,
		std::uint32_t const MaxFilter = 0,
		std::uint32_t const Option = PM_REMOVE
	) noexcept
	{
		return PeekMessageW(MessageData, WindowHandle, MinFilter, MaxFilter, Option) != 0;
	}

	// Removes messages from the thread message queue.
	// Both thread and window messages are processed.
	// Note that Windows prevents windows from being created if there is a WM_QUIT message present in the message queue.
	inline void clear_thread_message_queue(std::uint32_t const MinFilter = 0, std::uint32_t const MaxFilter = 0) noexcept
	{
		MSG msg;
		while (PeekMessageW(&msg, nullptr, MinFilter, MaxFilter, PM_REMOVE) != 0);
	}

	// Returns the width and height of the client area for the specified window.
	// This function wraps the GetClientRect function. For further reading, view the MSDN documentation for GetClientRect.
	[[nodiscard]] inline vec2i get_window_client_size(_In_ HWND const WindowHandle)
	{
		RECT rc;
		if (GetClientRect(WindowHandle, &rc) == 0)
		{
			throw_last_error();
		}
		return { rc.right, rc.bottom };
	}

	inline void dirty_window(_In_ HWND const Hwnd, bool const EraseBackground = false)
	{
		// Unlike InvalidateRect, dirty_window does not accept a NULL window handle, as this use is obscure and often unintended.
		WDUL_ASSERT(Hwnd != nullptr);
		if (InvalidateRect(Hwnd, nullptr, EraseBackground) == 0)
		{
			throw_last_error(WDUL_DEBUG_SWITCH("InvalidateRect failed",));
		}
	}

	inline void dirty_window(_In_ HWND const Hwnd, recti const& Rect, bool const EraseBackground = false)
	{
		// Unlike InvalidateRect, dirty_window does not accept a NULL window handle, as this use is obscure and often unintended.
		WDUL_ASSERT(Hwnd != nullptr);
		if (InvalidateRect(Hwnd, std::bit_cast<RECT const*>(&Rect), EraseBackground) == 0)
		{
			throw_last_error(WDUL_DEBUG_SWITCH("InvalidateRect failed", ));
		}
	}
}
