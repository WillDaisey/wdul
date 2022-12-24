// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "error.hpp"
#include "debug.hpp"

namespace wdul
{
	template <class T>
	concept handle_traits = requires(typename T::value_type Ptr)
	{
		{ T::invalid_value } -> std::convertible_to<typename T::value_type>;
		{ T::close(Ptr) } noexcept -> std::same_as<bool>;
	};

	template <handle_traits T>
	class handle
	{
		handle(handle const&) = delete;
		handle& operator=(handle const&) = delete;
	public:
		using traits = T;
		using value_type = traits::value_type;

		handle() noexcept : mValue(traits::invalid_value)
		{
		}

		explicit handle(value_type const Value) noexcept : mValue(Value)
		{
		}

		handle(handle&& Other) noexcept : mValue(Other.detach())
		{
		}

		handle& operator=(handle&& Other) noexcept
		{
			if (this != &Other)
			{
				attach(Other.detach());
			}
			return *this;
		}

		~handle()
		{
			if (*this)
			{
#ifdef _DEBUG
				try
				{
					check_bool(traits::close(mValue));
				}
				catch (std::exception const& e)
				{
					debug::output(
						debug::get_facility(),
						debug::categories::close_handle,
						debug::severity::warn,
						__func__,
						e.what()
					);
				}
#else
				traits::close(mValue);
#endif
			}
		}

		bool try_close() noexcept
		{
			if (*this)
			{
				bool const result = traits::close(mValue);
				mValue = traits::invalid_value;
				return result;
			}
			return true;
		}

		void close()
		{
			check_bool(try_close());
		}

		explicit operator bool() const noexcept
		{
			return mValue != traits::invalid_value;
		}

		[[nodiscard]] value_type get() const noexcept
		{
			return mValue;
		}

		[[nodiscard]] value_type* put() noexcept
		{
			// Ensure put() does not overwrite a valid handle.
			WDUL_ASSERT(mValue == traits::invalid_value);
			return &mValue;
		}

		[[nodiscard]] value_type& put_ref() noexcept
		{
			WDUL_ASSERT(mValue == traits::invalid_value);
			return mValue;
		}

		[[nodiscard]] value_type* reput()
		{
			close();
			return &mValue;
		}

		[[nodiscard]] value_type& reput_ref()
		{
			close();
			return mValue;
		}

		void attach(value_type const Value)
		{
			close();
			mValue = Value;
		}

		// Relenquish ownership of the underlying handle.
		value_type detach() noexcept
		{
			return std::exchange(mValue, traits::invalid_value);
		}

		void swap(handle& Rhs) noexcept
		{
			using std::swap;
			swap(mValue, Rhs.mValue);
		}

	private:
		value_type mValue;
	};

	template <class T>
	void swap(handle<T>& Lhs, handle<T>& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	enum class invalid_handle_type : bool
	{
		null,
		invalid_handle_value,
	};

	template <invalid_handle_type>
	struct generic_handle_traits
	{
		using value_type = void*;
		static value_type constexpr invalid_value = INVALID_HANDLE_VALUE;

		static bool close(value_type const Value) noexcept
		{
			return CloseHandle(Value) != 0;
		}
	};

	template<>
	struct generic_handle_traits<invalid_handle_type::null>
	{
		using value_type = void*;
		static value_type constexpr invalid_value = nullptr;

		static bool close(value_type const Value) noexcept
		{
			return CloseHandle(Value) != 0;
		}
	};

	template <invalid_handle_type Nullifier>
	using generic_handle = handle<generic_handle_traits<Nullifier>>;

	template <handle_traits T>
	handle<T> check_handle(typename T::value_type Handle, std::string_view const Message = {})
	{
		if (Handle == T::invalid_value)
		{
			throw_last_error(Message);
		}
		return handle<T>(Handle);
	}
}
