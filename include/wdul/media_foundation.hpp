// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/
#pragma once
#include "error.hpp"
#include <mfapi.h>

namespace wdul
{
	// Wraps the MFStartup and MFShutdown functions.
	class media_foundation
	{
	public:
		media_foundation(media_foundation const&) = delete;
		media_foundation& operator=(media_foundation const&) = delete;

		explicit media_foundation(std::uint32_t const Version = MF_VERSION, std::uint32_t const Flags = MFSTARTUP_FULL);

		media_foundation(media_foundation&& Other) noexcept :
			mInitialized(std::exchange(Other.mInitialized, false))
		{
		}

		~media_foundation();

		media_foundation& operator=(media_foundation&& Other) noexcept;

		void swap(media_foundation& Other) noexcept
		{
			std::swap(mInitialized, Other.mInitialized);
		}

		[[nodiscard]] bool initialized() const noexcept
		{
			return mInitialized;
		}

	private:
		void mf_shutdown() noexcept;
		bool mInitialized;
	};

	inline void swap(media_foundation& Lhs, media_foundation& Rhs) noexcept
	{
		Lhs.swap(Rhs);
	}
}
