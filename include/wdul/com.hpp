// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "counted_ptr.hpp"
#include "error.hpp"

namespace wdul
{
	// Wraps the CoInitializeEx and CoUninitialize functions.
	class coinit_scope
	{
		coinit_scope(const coinit_scope&) = delete;
		coinit_scope& operator=(const coinit_scope&) = delete;
		coinit_scope(coinit_scope&&) = delete;
		coinit_scope& operator=(coinit_scope&&) = delete;
	public:
		explicit coinit_scope(std::uint32_t const Flags)
		{
			check_hresult(CoInitializeEx(nullptr, Flags));
		}

		~coinit_scope() noexcept
		{
			CoUninitialize();
		}
	};

	template <class T>
	struct com_referencer
	{
		using value_type = T;

		static void ref(value_type* const Source) noexcept
		{
			Source->AddRef();
		}

		static void unref(value_type* const Source) noexcept
		{
			Source->Release();
		}
	};

	// Manages a COM interface pointer.
	template <class T>
	class com_ptr : public basic_counted_ptr<com_referencer<T>>
	{
		using base_type = basic_counted_ptr<com_referencer<T>>;
	public:
		using base_type::value_type;
		using base_type::base_type;

		template <class ToT>
		[[nodiscard]] HRESULT try_as(ToT** const Ppi) const noexcept
		{
			if constexpr (std::is_same_v<ToT, value_type>)
			{
				this->copy_to(Ppi);
				return 0;
			}
			else
			{
				return this->get()->QueryInterface(Ppi);
			}
		}

		template <class ToT>
		void as(ToT** const Ppi) const
		{
			check_hresult(this->try_as(Ppi));
		}
	};
}
