// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "memory.hpp"

namespace wdul
{
	// Where T has the ref and unref member functions.
	template <class T>
	struct default_referencer
	{
		using value_type = T;

		static void ref(value_type* const Source) noexcept(std::is_nothrow_invocable_v<decltype(&value_type::ref), value_type>)
		{
			Source->ref();
		}

		static void unref(value_type* const Source) noexcept(std::is_nothrow_invocable_v<decltype(&value_type::unref), value_type>)
		{
			Source->unref();
		}
	};

	namespace impl
	{
		template <class T>
		concept has_access_reference = requires
		{
			typename T::access_reference;
		};

		template <class T>
		concept has_access_pointer = requires
		{
			typename T::access_pointer;
		};

		template <class T, bool = has_access_reference<T>>
		struct access_reference_sfinae
		{
			using type = T::value_type&;
		};

		template <class T>
		struct access_reference_sfinae<T, true>
		{
			using type = T::access_reference;
		};

		template <class T, bool = has_access_pointer<T>>
		struct access_pointer_sfinae
		{
			using type = T::value_type*;
		};

		template <class T>
		struct access_pointer_sfinae<T, true>
		{
			using type = T::access_pointer;
		};
	}

	template <class ReferencerT>
	struct referencer_traits
	{
		using referencer_type = ReferencerT;
		using value_type = referencer_type::value_type;
		//using access_pointer = impl::access_pointer_sfinae<referencer_type>::type;
		//using access_reference = impl::access_reference_sfinae<referencer_type>::type;
		
		static bool constexpr nothrow_refable = std::is_nothrow_invocable_v<decltype(&referencer_type::ref), value_type*>;
		static bool constexpr nothrow_unrefable = std::is_nothrow_invocable_v<decltype(&referencer_type::unref), value_type*>;
		static bool constexpr nothrow_rerefable = nothrow_refable && nothrow_unrefable;

		static value_type* ref(value_type* const Source) noexcept(nothrow_refable)
		{
			if (Source)
			{
				referencer_type::ref(Source);
			}
			return Source;
		}

		static value_type* ref_unchecked(value_type* const Source) noexcept(nothrow_refable)
		{
			referencer_type::ref(Source);
			return Source;
		}

		static void unref(value_type*& Source) noexcept(nothrow_unrefable)
		{
			if (Source)
			{
				referencer_type::unref(std::exchange(Source, {}));
			}
		}

		static void unref_unchecked(value_type*& Source) noexcept(nothrow_unrefable)
		{
			referencer_type::unref(std::exchange(Source, {}));
		}

		static void copy_ref(value_type*& Dest, value_type* const Source) noexcept(nothrow_rerefable)
		{
			if (Dest != Source)
			{
				unref(Dest);
				Dest = ref(Source);
			}
		}
	};

	template <class ReferencerT>
	class basic_counted_ptr
	{
		template <class>
		friend class basic_counted_ptr;
	public:
		using traits = referencer_traits<ReferencerT>;
		using value_type = traits::value_type;
		//using access_pointer = traits::access_pointer;
		//using access_reference = traits::access_reference;

		static_assert(traits::nothrow_unrefable);

		basic_counted_ptr(std::nullptr_t = nullptr) noexcept :
			mPtr{}
		{
		}

		basic_counted_ptr(value_type* const Ptr, take_ownership_t) noexcept :
			mPtr(Ptr)
		{
		}

		explicit basic_counted_ptr(value_type* const Ptr) noexcept(traits::nothrow_refable) :
			mPtr(traits::ref(Ptr))
		{
		}

		explicit basic_counted_ptr(value_type& Ref) noexcept :
			mPtr(traits::ref_unchecked(&Ref))
		{
		}

		basic_counted_ptr(basic_counted_ptr const& Other) noexcept(traits::nothrow_refable) :
			mPtr(traits::ref(Other.mPtr))
		{
		}

		template <class U>
		basic_counted_ptr(basic_counted_ptr<U> const& Other) noexcept(traits::nothrow_refable) :
			mPtr(traits::ref(Other.mPtr))
		{
		}

		template <class U>
		basic_counted_ptr(basic_counted_ptr<U>&& Other) noexcept(traits::nothrow_refable) :
			mPtr(std::exchange(Other.mPtr, {}))
		{
		}

		~basic_counted_ptr()
		{
			traits::unref(mPtr);
		}

		template <class U>
		basic_counted_ptr& operator=(basic_counted_ptr<U> const& Other) noexcept(traits::nothrow_rerefable)
		{
			copy_from(Other.mPtr);
			return *this;
		}

		template <class U>
		basic_counted_ptr& operator=(basic_counted_ptr<U>&& Other) noexcept
		{
			auto temp = std::exchange(Other.mPtr, {});
			traits::unref(mPtr);
			mPtr = temp;
			return *this;
		}

		basic_counted_ptr& operator=(std::nullptr_t) noexcept
		{
			traits::unref(mPtr);
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return mPtr != nullptr;
		}

		[[nodiscard]] value_type* operator->() const noexcept
		{
			return mPtr;
		}

		[[nodiscard]] value_type& operator*() const noexcept
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

		[[nodiscard]] value_type** reput() noexcept
		{
			*this = nullptr;
			return &mPtr;
		}

		[[nodiscard]] value_type*& put_ref() noexcept
		{
			WDUL_ASSERT(mPtr == nullptr);
			return mPtr;
		}

		[[nodiscard]] value_type*& reput_ref() noexcept
		{
			*this = nullptr;
			return mPtr;
		}

		void attach(value_type* const Ptr) noexcept
		{
			traits::unref(mPtr);
			mPtr = Ptr;
		}

		value_type* detach() noexcept
		{
			return std::exchange(mPtr, {});
		}

		void swap(basic_counted_ptr& Rhs) noexcept
		{
			using std::swap;
			swap(mPtr, Rhs.mPtr);
		}

		void copy_from(value_type* const Other) noexcept(traits::nothrow_rerefable)
		{
			traits::copy_ref(mPtr, Other);
		}

		void copy_to(value_type** const Other) const noexcept(traits::nothrow_refable)
		{
			*Other = traits::ref(mPtr);
		}

		void copy_to(value_type*& Other) const noexcept(traits::nothrow_refable)
		{
			Other = traits::ref(mPtr);
		}

		[[nodiscard]] auto operator<=>(basic_counted_ptr const&) const noexcept = default;

	private:
		value_type* mPtr;
	};

	template <class ReferencerT>
	void swap(basic_counted_ptr<ReferencerT>& Lhs, basic_counted_ptr<ReferencerT>& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	template <class T>
	using counted_ptr = basic_counted_ptr<default_referencer<T>>;
}
