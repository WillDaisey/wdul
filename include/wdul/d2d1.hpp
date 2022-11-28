// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "dxgi.hpp"
#include "graphics_common.hpp"
#include <d2d1.h>

namespace wdul
{
	template <class T>
	void d2d1_create_factory(_Outptr_ T** const Factory, D2D1_FACTORY_TYPE const Type, D2D1_DEBUG_LEVEL const DebugLevel = WDUL_DEBUG_SWITCH(D2D1_DEBUG_LEVEL_WARNING, D2D1_DEBUG_LEVEL_NONE))
	{
		D2D1_FACTORY_OPTIONS options{};
		options.debugLevel = DebugLevel;
		check_hresult(D2D1CreateFactory<T>(Type, options, Factory));
	}

	// Wraps BeginDraw/EndDraw calls for a given ID2D1RenderTarget.
	// Note that d2d1_draw_scope does not modify the reference count for a given ID2D1RenderTarget, so don't release the
	// render target until the d2d1_draw_scope has called ID2D1RenderTarget::EndDraw.
	class d2d1_draw_scope
	{
	public:
		d2d1_draw_scope(d2d1_draw_scope const&) = delete;
		d2d1_draw_scope(d2d1_draw_scope&&) = delete;
		d2d1_draw_scope& operator=(d2d1_draw_scope) = delete;

		// Initiates drawing with Direct2D by calling BeginDraw on the ID2D1RenderTarget.
		explicit d2d1_draw_scope(_In_ ID2D1RenderTarget* const Rt) noexcept :
			mRt(Rt)
		{
			Rt->BeginDraw();
		}

		// Provided that d2d1_draw_scope::end has not been called:
		// Ends Direct2D drawing operations by calling EndDraw on the ID2D1RenderTarget.
		// Any error code returned by EndDraw is ignored.
		// Because error codes are ignored, it is not recommended to end drawing in this way unless an exception was thrown.
		~d2d1_draw_scope()
		{
			if (mRt)
			{
				mRt->EndDraw();
			}
		}

		// Ends Direct2D drawing operations by calling EndDraw on the ID2D1RenderTarget.
		// If EndDraw fails, an exception is thrown unless EndDraw returns D2DERR_RECREATE_TARGET.
		// Returns graphics_device_presence::ready if EndDraw succeeded.
		// Returns graphics_device_presence::lost if EndDraw failed because the device was lost.
		// A caller may choose to ignore the return value of this function if device loss is handled at a later time.
		graphics_device_presence end(_Out_opt_ D2D1_TAG* Tag1 = nullptr, _Out_opt_ D2D1_TAG* Tag2 = nullptr)
		{
			auto const hr = mRt->EndDraw(Tag1, Tag2);
			mRt = nullptr;
			if (FAILED(hr))
			{
				if (hr == D2DERR_RECREATE_TARGET) return graphics_device_presence::lost;
				throw_hresult(hr);
			}
			return graphics_device_presence::ready;
		}

	private:
		ID2D1RenderTarget* mRt;
	};
}
