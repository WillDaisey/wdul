// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "com.hpp"
#include "thread.hpp"
#include <xaudio2.h>

namespace wdul
{
	template <class T>
	concept xaudio2_voice_interface = std::is_base_of_v<IXAudio2Voice, T>;

	template <class T>
	concept xaudio2_engine_callback_interface = std::is_base_of_v<IXAudio2EngineCallback, T>;

	template <class T>
	concept xaudio2_voice_callback_interface = std::is_base_of_v<IXAudio2VoiceCallback, T>;

	template <xaudio2_voice_interface T = IXAudio2Voice>
	class xaudio2_voice_ptr
	{
		template <xaudio2_voice_interface>
		friend class voice_ptr;

	public:
		xaudio2_voice_ptr(xaudio2_voice_ptr const&) = delete;
		xaudio2_voice_ptr& operator=(xaudio2_voice_ptr const&) = delete;

		using value_type = T;

		xaudio2_voice_ptr(std::nullptr_t = nullptr) noexcept : mPtr{}
		{
		}

		explicit xaudio2_voice_ptr(value_type* const Ptr) noexcept : mPtr(Ptr)
		{
		}

		template <class U>
		xaudio2_voice_ptr(xaudio2_voice_ptr<U>&& Other) noexcept : mPtr(std::exchange(Other.mPtr, nullptr))
		{
		}

		~xaudio2_voice_ptr() noexcept
		{
			if (mPtr)
			{
				std::exchange(mPtr, {})->DestroyVoice();
			}
		}

		template <class U>
		xaudio2_voice_ptr& operator=(xaudio2_voice_ptr<U>&& Other) noexcept
		{
			auto temp = std::exchange(Other.mPtr, nullptr);
			if (mPtr)
			{
				std::exchange(mPtr, nullptr)->DestroyVoice();
			}
			mPtr = temp;
		}

		explicit operator bool() const noexcept
		{
			return mPtr != nullptr;
		}

		value_type* operator->() const noexcept
		{
			return mPtr;
		}

		value_type& operator*() const noexcept
		{
			return *mPtr;
		}

		[[nodiscard]] value_type* get() const noexcept
		{
			return mPtr;
		}

		[[nodiscard]] value_type** put() noexcept
		{
			WDUL_ASSERT(mPtr == nullptr);
			return &mPtr;
		}

		[[nodiscard]] void** put_void() noexcept
		{
			return reinterpret_cast<void**>(put());
		}

		void attach(value_type* const Ptr) noexcept
		{
			if (mPtr)
			{
				mPtr->DestroyVoice();
			}
			mPtr = Ptr;
		}

		value_type* detach() noexcept
		{
			return std::exchange(mPtr, {});
		}

		void swap(xaudio2_voice_ptr& Rhs) noexcept
		{
			using std::swap;
			swap(mPtr, Rhs.mPtr);
		}

		auto operator<=>(xaudio2_voice_ptr const&) const noexcept = default;

	private:
		value_type* mPtr;
	};

	template <xaudio2_voice_interface T>
	void swap(xaudio2_voice_ptr<T>& Lhs, xaudio2_voice_ptr<T>& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	using xaudio2_mastering_voice_ptr = xaudio2_voice_ptr<IXAudio2MasteringVoice>;
	using xaudio2_source_voice_ptr = xaudio2_voice_ptr<IXAudio2SourceVoice>;
	using xaudio2_submix_voice_ptr = xaudio2_voice_ptr<IXAudio2SubmixVoice>;

	class xaudio2_engine_callback : public IXAudio2EngineCallback
	{
	public:
		xaudio2_engine_callback(xaudio2_engine_callback const&) = delete;
		xaudio2_engine_callback& operator=(xaudio2_engine_callback const&) = delete;

		xaudio2_engine_callback() = default;
		xaudio2_engine_callback(xaudio2_engine_callback&&) noexcept = default;
		xaudio2_engine_callback& operator=(xaudio2_engine_callback&&) noexcept = default;
#pragma warning(suppress:4265) // class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
		~xaudio2_engine_callback() noexcept = default;

		void critical_error(std::exception_ptr const Exception) noexcept
		{
			mCriticalError = Exception;
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(SetEvent(mCriticalErrorEvent.get()), != 0);
		}

		[[nodiscard]] auto critical_error() const noexcept { return mCriticalError; }
		[[nodiscard]] auto critical_error_event() const noexcept { return mCriticalErrorEvent.get(); }

	private:
		// Called by XAudio2 just before an audio processing pass begins.
		void __stdcall OnProcessingPassStart() noexcept override {}

		// Called just after an audio processing pass ends.
		void __stdcall OnProcessingPassEnd() noexcept override {}

		// Called in the event of a critical system error which requires XAudio2
		// to be closed down and restarted. The error code is given in Error.
		void __stdcall OnCriticalError(HRESULT const Error) noexcept override
		{
#ifdef _DEBUG
			debug::output(debug::get_facility(), debug::categories::xaudio2, debug::severity::warn,
				__func__, hresult_error(Error).what());
#endif
			try
			{
				throw_hresult(Error);
			}
			catch (...)
			{
				critical_error(std::current_exception());
			}
		}

		std::exception_ptr mCriticalError;
		event_handle mCriticalErrorEvent = create_event(event_access_mask(standard_access::synchronize, event_access::modify_state));
	};

	template <xaudio2_engine_callback_interface T = xaudio2_engine_callback>
	class xaudio2_engine_callback_registration
	{
	public:
		xaudio2_engine_callback_registration(xaudio2_engine_callback_registration const&) = delete;
		xaudio2_engine_callback_registration& operator=(xaudio2_engine_callback_registration const&) = delete;

		xaudio2_engine_callback_registration() noexcept :
			mCallback{}
		{
		}

		explicit xaudio2_engine_callback_registration(_In_ IXAudio2* const Xaudio2) noexcept :
			mXaudio2(ref_unchecked<auto, com_referencer<IXAudio2>>(Xaudio2), take_ownership), mCallback{}
		{
		}

		xaudio2_engine_callback_registration(_In_ IXAudio2* const Xaudio2, _In_ T* const Callback) :
			mXaudio2(ref_unchecked<auto, com_referencer<IXAudio2>>(Xaudio2), take_ownership), mCallback(Callback)
		{
			check_hresult(mXaudio2->RegisterForCallbacks(Callback));
		}

		xaudio2_engine_callback_registration(xaudio2_engine_callback_registration&& Other) noexcept :
			mXaudio2(std::move(Other.mXaudio2)), mCallback(std::exchange(Other.mCallback, {}))
		{
		}

		xaudio2_engine_callback_registration& operator=(xaudio2_engine_callback_registration&& Other) noexcept
		{
			auto tmpXaudio2 = Other.mXaudio2.detach();
			auto tmpCallback = std::exchange(Other.mCallback, nullptr);
			unregister();
			mXaudio2.put_ref() = tmpXaudio2;
			mCallback = tmpCallback;
			return *this;
		}

		~xaudio2_engine_callback_registration()
		{
			unregister();
		}

		void initialize(_In_ IXAudio2* const Xaudio2, _In_ T* const Callback)
		{
			WDUL_ASSERT(mCallback == nullptr);
			mXaudio2.copy_from(Xaudio2);
			check_hresult(mXaudio2->RegisterForCallbacks(Callback));
			mCallback = Callback;

		}

		void reset(_In_ IXAudio2* const Xaudio2, _In_ T* const Callback)
		{
			unregister();
			initialize(Xaudio2, Callback);
		}

		void unregister() noexcept
		{
			if (mXaudio2 && mCallback)
			{
				mXaudio2->UnregisterForCallbacks(mCallback);
				mCallback = {};
			}
		}

		[[nodiscard]] auto xaudio2() const noexcept { return mXaudio2.get(); }
		[[nodiscard]] auto callback() const noexcept { return mCallback; }

	private:
		com_ptr<IXAudio2> mXaudio2;
		T* mCallback;
	};

	class xaudio2_voice_callback : public IXAudio2VoiceCallback
	{
	public:
		xaudio2_voice_callback(xaudio2_voice_callback const&) = delete;
		xaudio2_voice_callback& operator=(xaudio2_voice_callback const&) = delete;

		xaudio2_voice_callback() = default;
		xaudio2_voice_callback(xaudio2_voice_callback&&) noexcept = default;
		xaudio2_voice_callback& operator=(xaudio2_voice_callback&&) noexcept = default;
#pragma warning(suppress:4265) // class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
		~xaudio2_voice_callback() = default;

		void error(std::exception_ptr const Exception) noexcept
		{
			mError = Exception;
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(SetEvent(mErrorEvent.get()), != 0);
		}

		[[nodiscard]] auto error() const noexcept { return mError; }
		[[nodiscard]] auto error_event() const noexcept { return mErrorEvent.get(); }
		[[nodiscard]] auto buffer_end_event() const noexcept { return mBufferEndEvent.get(); }

	private:
		// Called just before this voice's processing pass begins.
		void __stdcall OnVoiceProcessingPassStart(UINT32 const) noexcept override {}

		// Called just after this voice's processing pass ends.
		void __stdcall OnVoiceProcessingPassEnd() noexcept override {}

		// Called when this voice has just finished playing a buffer stream
		// (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
		void __stdcall OnStreamEnd() noexcept override {}

		// Called when this voice is about to start processing a new buffer.
		void __stdcall OnBufferStart(void* const) noexcept override {}

		// Called when this voice has just finished processing a buffer.
		// The buffer can now be reused or destroyed.
		void __stdcall OnBufferEnd(void* const Context) noexcept override
		{
			if (SetEvent(mBufferEndEvent.get()) == 0)
			{
				OnVoiceError(Context, HRESULT_FROM_WIN32(GetLastError()));
			}
		}

		// Called when this voice has just reached the end position of a loop.
		void __stdcall OnLoopEnd(void* const) noexcept override {}

		// Called in the event of a critical error during voice processing,
		// such as a failing xAPO or an error from the hardware XMA decoder.
		// The voice may have to be destroyed and re-created to recover from
		// the error.  The callback arguments report which buffer was being
		// processed when the error occurred, and its HRESULT code.
		void __stdcall OnVoiceError(void* const, HRESULT const Error) noexcept override
		{
#ifdef _DEBUG
			debug::output(debug::get_facility(), debug::categories::xaudio2, debug::severity::warn,
				__func__, hresult_error(Error).what());
#endif
			try
			{
				throw_hresult(Error);
			}
			catch (...)
			{
				error(std::current_exception());
			}
		}

		std::exception_ptr mError;
		event_handle mErrorEvent = create_event(access_mask(standard_access::synchronize, event_access::modify_state));
		event_handle mBufferEndEvent = create_event(access_mask(standard_access::synchronize, event_access::modify_state));
	};
}
