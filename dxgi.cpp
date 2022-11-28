// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/dxgi.hpp"

namespace wdul
{
	[[nodiscard]] std::vector<com_ptr<IDXGIAdapter4>> dxgi_list_adapters(
		_In_ IDXGIFactory6* const DxgiFactory,
		DXGI_GPU_PREFERENCE const GpuPreference,
		bool const NoSoftwareAdapter
	)
	{
		std::vector<com_ptr<IDXGIAdapter4>> adapterArray;
		adapterArray.reserve(3);
		HRESULT hr;
		for (std::uint32_t adapterIndex = 0; true; ++adapterIndex)
		{
			com_ptr<IDXGIAdapter4> adapter;
			hr = DxgiFactory->EnumAdapterByGpuPreference(adapterIndex, GpuPreference, IID_PPV_ARGS(adapter.put()));
			if (FAILED(hr))
			{
				if (hr == DXGI_ERROR_NOT_FOUND)
				{
					return adapterArray;
				}
				throw_hresult(hr);
			}

			if (NoSoftwareAdapter)
			{
				DXGI_ADAPTER_DESC1 desc;
				check_hresult(adapter->GetDesc1(&desc));
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}
			}

			adapterArray.push_back(std::move(adapter));
		}
	}

	dxgi_fullscreen_transition_result dxgi_set_fullscreen_state(_In_ IDXGISwapChain* const SwapChain, bool const Enable, _In_opt_ IDXGIOutput* const Target)
	{
		auto const hr = SwapChain->SetFullscreenState(Enable, Target);
		if (FAILED(hr))
		{
			switch (hr)
			{
			case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
				return dxgi_fullscreen_transition_result::unavailable;
			case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS:
				return dxgi_fullscreen_transition_result::transition_in_progress;
			default:
				throw_hresult(hr, "SetFullscreenState failed");
			}
		}
		return dxgi_fullscreen_transition_result::success;
	}
}
