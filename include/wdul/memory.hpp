// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "foundation.hpp"
#include <cstring>
#include <new>
#include <memory>
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
	// take_ownership_t indicates that a function takes ownership of the given resource(s).
	struct take_ownership_t {};
	inline constexpr take_ownership_t take_ownership{};

	template <class T>
	concept allocate_traits = requires(void* const Ptr, std::size_t const Size)
	{
		// Size must be non-zero.
		{ T::allocate(Size) } -> std::same_as<void*>;
		// Size must be non-zero.
		{ T::allocate_zeroed(Size) } -> std::same_as<void*>;
		// Ptr may be nullptr, in which case, the function does nothing.
		{ T::deallocate(Ptr) } -> std::same_as<void>;
		// Ptr may be nullptr, in which case, the function performs an allocation. Size must be non-zero.
		{ T::reallocate(Ptr, Size) } -> std::same_as<void*>;
		// Ptr may be nullptr, in which case, the function performs a zeroed allocation. Size must be non-zero.
		{ T::reallocate_zeroed(Ptr, Size) } -> std::same_as<void*>;
	};

	template <class T>
	concept has_deallocate_unchecked = requires(void* const Ptr)
	{
		// Ptr must not be nullptr.
		{ T::deallocate_unchecked(Ptr) } -> std::same_as<void>;
	};

	template <class T>
	concept has_reallocate_unchecked = requires(void* const Ptr, std::size_t const Size)
	{
		// Ptr must not be nullptr. Size must be non-zero.
		{ T::reallocate_unchecked(Ptr, Size) } -> std::same_as<void*>;
	};

	template <class T>
	concept has_expand = requires(void* const Ptr, std::size_t const Size)
	{
		// Ptr may be nullptr, in which case, the function returns nullptr. Size must be non-zero, even if Ptr is nullptr.
		{ T::expand(Ptr, Size) } -> std::same_as<void*>;
	};

	template <class T>
	concept has_expand_unchecked = requires(void* const Ptr, std::size_t const Size)
	{
		// Ptr must not be nullptr. Size must be non-zero.
		{ T::expand_unchecked(Ptr, Size) } -> std::same_as<void*>;
	};

	// Provides a uniform interface for types which satisfy allocate_traits.
	template <allocate_traits AllocateTraits>
	struct allocator
	{
		using traits_type = AllocateTraits;

		// Allocates storage for Size bytes.
		// Size must be non-zero.
		[[nodiscard]] static void* allocate(std::size_t const Size)
		{
			return traits_type::allocate(Size);
		}

		// Allocates storage for Size bytes.
		// The requested portion of the allocated storage will be filled with zeroes.
		// Size must be non-zero.
		[[nodiscard]] static void* allocate_zeroed(std::size_t const Size)
		{
			return traits_type::allocate_zeroed(Size);
		}

		// Deallocates the storage pointed to by Ptr.
		// If Ptr is nullptr, the function does nothing.
		static void deallocate(void* const Ptr) noexcept
		{
			traits_type::deallocate(Ptr);
		}

		// Deallocates the storage pointed to by Ptr.
		// Ptr must not be nullptr.
		static void deallocate_unchecked(void* const Ptr) noexcept
		{
			if constexpr (has_deallocate_unchecked<traits_type>)
			{
				traits_type::deallocate_unchecked(Ptr);
			}
			else
			{
				WDUL_ASSERT(Ptr != nullptr);
				traits_type::deallocate(Ptr);
			}
		}

		// Reallocates the storage pointed to by Ptr such that the new storage size is at least Size bytes.
		// If Ptr is nullptr, the function performs an allocation.
		// Size must be non-zero.
		static void* reallocate(void* const Ptr, std::size_t const Size)
		{
			return traits_type::reallocate(Ptr, Size);
		}

		// Reallocates the storage pointed to by Ptr such that the new storage size is at least Size bytes.
		// Ptr must not be nullptr.
		// Size must be non-zero.
		static void reallocate_unchecked(void* const Ptr, std::size_t const Size)
		{
			if constexpr (has_reallocate_unchecked<traits_type>)
			{
				return traits_type::reallocate_unchecked(Ptr, Size);
			}
			else
			{
				return traits_type::reallocate(Ptr, Size);
			}
		}

		// Reallocates the storage pointed to by Ptr such that the new storage size is at least Size bytes.
		// If the requested size is greater than the old size of the pointed-to storage, the portion beyond the old size will be initialised to zero.
		// The region of memory beyond the requested size may not be initialised.
		// If Ptr is nullptr, the function performs the equivalent of allocate_zeroed.
		// Size must be non-zero.
		static void* reallocate_zeroed(void* const Ptr, std::size_t const Size)
		{
			return traits_type::reallocate_zeroed(Ptr, Size);
		}
	};

	struct malloc_traits
	{
		[[nodiscard]] static void* allocate(std::size_t const Size)
		{
			WDUL_ASSERT(Size != 0);
			auto const p = std::malloc(Size);
			if (!p) throw std::bad_alloc();
			return p;
		}

		[[nodiscard]] static void* allocate_zeroed(std::size_t const Size)
		{
			WDUL_ASSERT(Size != 0);
			auto const p = std::malloc(Size);
			if (!p) throw std::bad_alloc();
			std::memset(p, 0, Size);
			return p;
		}

		static void deallocate(void* const Ptr) noexcept
		{
			std::free(Ptr);
		}

		[[nodiscard]] static void* reallocate(void* const Ptr, std::size_t const Size)
		{
			WDUL_ASSERT(Size != 0);
			auto const p = std::realloc(Ptr, Size);
			if (!p) throw std::bad_alloc();
			return p;
		}

		[[nodiscard]] static void* reallocate_zeroed(void* const Ptr, std::size_t const Size)
		{
			WDUL_ASSERT(Size != 0);
			auto const p = std::realloc(Ptr, Size);
			if (!p) throw std::bad_alloc();
			std::memset(p, 0, Size);
			return p;
		}
	};
	using mallocator = allocator<malloc_traits>;

	struct local_alloc_traits
	{
		[[nodiscard]] static void* allocate(std::size_t const Size)
		{
			WDUL_ASSERT(Size != 0);
			return LocalAlloc(0, Size);
		}

		[[nodiscard]] static void* allocate_zeroed(std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			return LocalAlloc(LMEM_ZEROINIT, Size);
		}

		static void deallocate(void* const Ptr) noexcept
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(LocalFree(Ptr), != nullptr);
		}

		[[nodiscard]] static void* reallocate(void* const Ptr, std::size_t const Size) noexcept
		{
			if (!Ptr)
			{
				return allocate(Size);
			}
			WDUL_ASSERT(Size != 0);
			return LocalReAlloc(Ptr, Size, LMEM_MOVEABLE);
		}

		[[nodiscard]] static void* reallocate_zeroed(void* const Ptr, std::size_t const Size) noexcept
		{
			if (!Ptr)
			{
				return allocate_zeroed(Size);
			}
			WDUL_ASSERT(Size != 0);
			return LocalReAlloc(Ptr, Size, LMEM_MOVEABLE | LMEM_ZEROINIT);
		}

		[[nodiscard]] static void* reallocate_fixed(void* const Ptr, std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			if (!Ptr)
			{
				return nullptr;
			}
			return LocalReAlloc(Ptr, Size, 0);
		}

		[[nodiscard]] static void* reallocate_fixed_zeroed(void* const Ptr, std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			if (!Ptr)
			{
				return nullptr;
			}
			return LocalReAlloc(Ptr, Size, LMEM_ZEROINIT);
		}

		[[nodiscard]] static void* expand(void* const Ptr, std::size_t const Size) noexcept
		{
			return reallocate_fixed(Ptr, Size);
		}
	};
	using local_allocator = allocator<local_alloc_traits>;

	struct heap_alloc_traits
	{
		[[nodiscard]] static void* allocate(std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			return HeapAlloc(GetProcessHeap(), 0, Size);
		}

		[[nodiscard]] static void* allocate_zeroed(std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
		}

		static void deallocate(void* const Ptr) noexcept
		{
			WDUL_DEBUG_RAISE_LAST_ERROR_WHEN(HeapFree(GetProcessHeap(), 0, Ptr), == 0);
		}

		[[nodiscard]] static void* reallocate(void* const Ptr, std::size_t const Size) noexcept
		{
			if (!Ptr)
			{
				return allocate(Size);
			}
			WDUL_ASSERT(Size != 0);
			return HeapReAlloc(GetProcessHeap(), 0, Ptr, Size);
		}

		[[nodiscard]] static void* reallocate_zeroed(void* const Ptr, std::size_t const Size) noexcept
		{
			if (!Ptr)
			{
				return allocate_zeroed(Size);
			}
			WDUL_ASSERT(Size != 0);
			return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Ptr, Size);
		}

		[[nodiscard]] static void* reallocate_fixed(void* const Ptr, std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			if (!Ptr)
			{
				return nullptr;
			}
			return HeapReAlloc(GetProcessHeap(), HEAP_REALLOC_IN_PLACE_ONLY, Ptr, Size);
		}

		[[nodiscard]] static void* reallocate_fixed_zeroed(void* const Ptr, std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			if (!Ptr)
			{
				return nullptr;
			}
			return HeapReAlloc(GetProcessHeap(), HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, Ptr, Size);
		}

		[[nodiscard]] static void* expand(void* const Ptr, std::size_t const Size) noexcept
		{
			WDUL_ASSERT(Size != 0);
			return reallocate_fixed(Ptr, Size);
		}
	};
	using heap_allocator = allocator<heap_alloc_traits>;

	// Returns the size, in bytes, of Count objects of type T.
	// Throws std::bad_array_new_length if the result of the calculation cannot fit into the unsigned integral type SizeT.
	template <cv_unqualified T, std::unsigned_integral SizeT = std::size_t>
	constexpr SizeT sizeof_n(SizeT const Count) noexcept(sizeof(T) <= 1)
	{
		if constexpr (sizeof(T) > 1)
		{
			if (Count > (std::numeric_limits<SizeT>::max)() / sizeof(T))
			{
				throw std::bad_array_new_length();
			}
		}
		return Count * sizeof(T);
	}

	/// <summary>Manages a dynamically allocated array of bytes. Stores a pointer to that array, and the size of that array.</summary>
	/// <typeparam name="AllocT">Allocator traits which specify how to allocate and deallocate storage.</typeparam>
	template <allocate_traits AllocT>
	class basic_byte_array
	{
	public:
		using allocator = allocator<AllocT>;

		/// <summary>Constructs a <c>basic_byte_array</c> with an empty data buffer.</summary>
		basic_byte_array() noexcept :
			mData(nullptr), mSize(0)
		{
		}

		/// <summary>
		/// Constructs a <c>basic_byte_array</c> with the given size and data buffer.
		/// <para>The <c>basic_byte_array</c> will take ownership of the given data buffer. Ensure the data buffer was allocated appropriately.</para>
		/// </summary>
		/// <param name="Size">The size of the data buffer pointed to by <paramref name="Data"/>.</param>
		/// <param name="Data">A pointer to the data buffer (an array of bytes), or <c>nullptr</c>.</param>
		/// <remarks>If <paramref name="Size"/> is zero, <paramref name="Data"/> must be <c>nullptr</c>, and vise-versa.</remarks>
		basic_byte_array(std::size_t const Size, _In_reads_opt_(Size) std::uint8_t* const Data, take_ownership_t) noexcept :
			mData(Data),
			mSize(Size)
		{
		}

		/// <summary>Constructs a <c>basic_byte_array</c> by copying the contents of <paramref name="Other"/>.</summary>
		/// <param name="Other">A reference to the object to copy data from.</param>
		basic_byte_array(basic_byte_array const& Other)
		{
			mSize = Other.mSize;
			if (mSize == 0)
			{
				mData = nullptr;
				return;
			}
			mData = static_cast<std::uint8_t*>(allocator::allocate(Other.mSize));
			std::memcpy(mData, Other.mData, mSize);
		}

		/// <summary>Constructs a <c>basic_byte_array</c> by moving the contents of <paramref name="Other"/>.</summary>
		/// <param name="Other">A reference to the object to move data from. The data buffer held by <paramref name="Other"/> will be emptied.</param>
		basic_byte_array(basic_byte_array&& Other) noexcept :
			mData(std::exchange(Other.mData, nullptr)),
			mSize(std::exchange(Other.mSize, 0))
		{
		}

		~basic_byte_array()
		{
			allocator::deallocate(mData);
		}

		/// <summary>Copies the contents of <paramref name="Other"/> to this object.</summary>
		/// <param name="Other">A reference to the object to copy data from.</param>
		/// <returns>A reference to this object.</returns>
		basic_byte_array& operator=(basic_byte_array const& Other)
		{
			std::uint8_t* newData = nullptr;
			if (Other.mSize != 0)
			{
				newData = static_cast<std::uint8_t*>(allocator::allocate(Other.mSize));
				std::memcpy(newData, Other.mData, Other.mSize);
			}
			allocator::deallocate(mData);
			mData = newData;
			mSize = Other.mSize;
			return *this;
		}

		/// <summary>
		/// Moves the contents of <paramref name="Other"/> to this object.
		/// <para>Self-assignment is handled by emptying the data buffer.</para>
		/// </summary>
		/// <param name="Other">A reference to the object to move data from. The data buffer held by <paramref name="Other"/> will be emptied.</param>
		/// <returns>A reference to this object.</returns>
		basic_byte_array& operator=(basic_byte_array&& Other) noexcept
		{
			auto const newData = std::exchange(Other.mData, nullptr);
			allocator::deallocate(mData);
			mData = newData;
			mSize = Other.mSize;
			Other.mSize = 0;
			return *this;
		}

		/// <summary>Swaps the contents of this object with that of <paramref name="Other"/>.</summary>
		/// <param name="Other">A reference to the object to swap data with.</param>
		void swap(basic_byte_array& Other)
		{
			std::swap(mData, Other.mData);
			std::swap(mSize, Other.mSize);
		}

		/// <returns>A pointer to the data buffer (an array of bytes).</returns>
		[[nodiscard]] auto data() const noexcept
		{
			return mData;
		}

		/// <returns>The size of the data buffer, in bytes.</returns>
		[[nodiscard]] auto size() const noexcept
		{
			return mSize;
		}

	private:
		std::uint8_t* mData;
		std::size_t mSize;
	};

	/// <summary>Swaps the contents of <paramref name="Lhs"/> with that of <paramref name="Rhs"/>.</summary>
	/// <typeparam name="AllocT">Allocator traits.</typeparam>
	/// <param name="Lhs">A reference to the first object.</param>
	/// <param name="Rhs">A reference to the second object.</param>
	template <allocate_traits AllocT>
	inline void swap(basic_byte_array<AllocT>& Lhs, basic_byte_array<AllocT>& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}

	using byte_array = basic_byte_array<malloc_traits>;
}
