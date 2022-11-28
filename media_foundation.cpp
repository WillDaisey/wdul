// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/media_foundation.hpp"
#include "include/wdul/foundation.hpp"

namespace wdul
{
	media_foundation::media_foundation(std::uint32_t const Version, std::uint32_t const Flags)
	{
		check_hresult(MFStartup(Version, Flags), "MFStartup failed");
		mInitialized = true;
	}

	media_foundation::~media_foundation()
	{
		if (mInitialized)
		{
			mf_shutdown();
		}
	}

	media_foundation& media_foundation::operator=(media_foundation&& Other) noexcept
	{
		// Self move-assignment does nothing.
		auto temp = std::exchange(Other.mInitialized, false);
		if (mInitialized)
		{
			mf_shutdown();
		}
		mInitialized = temp;
		return *this;
	}

	void media_foundation::mf_shutdown() noexcept
	{
		WDUL_ASSERT(mInitialized);
#ifdef _DEBUG
		try
		{
			check_hresult(MFShutdown(), "MFShutdown failed");
		}
		catch (std::exception const& e)
		{
			WDUL_WARN("WDUL", e.what());
		}
#else
		MFShutdown();
#endif
	}
}
