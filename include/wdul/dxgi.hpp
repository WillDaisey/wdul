// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "com.hpp"
#include <vector>
#include <dxgi1_6.h>

namespace wdul
{
	template <class T>
	void dxgi_create_factory(_Outptr_ T** const Factory, std::uint32_t const Flags = WDUL_DEBUG_SWITCH(DXGI_CREATE_FACTORY_DEBUG, 0))
	{
		check_hresult(CreateDXGIFactory2(Flags, IID_PPV_ARGS(Factory)));
	}

	inline DXGI_GPU_PREFERENCE constexpr dxgi_default_gpu_preference = WDUL_DEBUG_SWITCH(DXGI_GPU_PREFERENCE_MINIMUM_POWER, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);

	// Returns a dynamic array of available graphics adapters. If no adapters were found, the returned array is empty.
	// GpuPreference specifies how to sort adapters:
	// If DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE is specified, the highest performing GPUs will be at the front of the list.
	// If DXGI_GPU_PREFERENCE_MINIMUM_POWER is specified, the minimum-powered GPUs will be at the front of the list.
	// NoSoftwareAdapter specifies whether to ignore the Basic Render Driver adapter (true) or not (false).
	[[nodiscard]] std::vector<com_ptr<IDXGIAdapter4>> dxgi_list_adapters(
		_In_ IDXGIFactory6* const DxgiFactory,
		DXGI_GPU_PREFERENCE const GpuPreference = dxgi_default_gpu_preference,
		bool const NoSoftwareAdapter = true
	);

	// Returns true if and only if the display supports tearing.
	// Internally calls IDXGIFactory5::CheckFeatureSupport with the feature DXGI_FEATURE_PRESENT_ALLOW_TEARING.
	[[nodiscard]] inline bool dxgi_check_tearing_support(_In_ IDXGIFactory5* const DxgiFactory)
	{
		std::uint32_t allowTearing{};
		check_hresult(DxgiFactory->CheckFeatureSupport(
			DXGI_FEATURE_PRESENT_ALLOW_TEARING,
			&allowTearing, sizeof(allowTearing)
		));
		return static_cast<bool>(allowTearing);
	}

	// Return values for dxgi_set_fullscreen_state.
	enum class dxgi_fullscreen_transition_result : std::uint8_t
	{
		// The swap chain was successfully placed in the requested state.
		success,

		// The swap chain failed to be placed in the requested state, but it may succeed later.
		// This error can occur when the output window does not have keyboard focus, or if the output window is occluded.
		unavailable,

		// A fullscreen/windowed mode transition is already occurring.
		transition_in_progress,
	};

	// Sets the display state to windowed or fullscreen by calling SwapChain->SetFullscreenState.
	// Returns dxgi_fullscreen_transition_result::success if the swap chain was successfully placed in the requested state.
	// Returns dxgi_fullscreen_transition_result::unavailable if SetFullscreenState returns DXGI_ERROR_NOT_CURRENTLY_AVAILABLE.
	// Returns dxgi_fullscreen_transition_result::transition_in_progress if SetFullscreenState returns DXGI_STATUS_MODE_CHANGE_IN_PROGRESS.
	// If SetFullscreenState returns any other error, it is thrown via throw_hresult.
	dxgi_fullscreen_transition_result dxgi_set_fullscreen_state(_In_ IDXGISwapChain* const SwapChain, bool const Enable, _In_opt_ IDXGIOutput* const Target = nullptr);
}
