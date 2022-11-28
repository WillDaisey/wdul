// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/d3d11.hpp"

namespace wdul
{
	void d3d11_create_device(
		_Outptr_ ID3D11Device** const Device,
		_Outptr_opt_ ID3D11DeviceContext** const ImmediateContext,
		_In_ IDXGIFactory6* const DxgiFactory,
		std::uint32_t const NumFeatureLevels,
		_In_reads_opt_(NumFeatureLevels) D3D_FEATURE_LEVEL const* const FeatureLevels,
		_Out_opt_ D3D_FEATURE_LEVEL* const FeatureLevel,
		std::uint32_t const Flags,
		DXGI_GPU_PREFERENCE const GpuPreference,
		bool const NoSoftwareAdapter,
		std::uint32_t const SdkVersion
	)
	{
		auto const adapters = dxgi_list_adapters(DxgiFactory, GpuPreference, NoSoftwareAdapter);
		auto hr = DXGI_ERROR_NOT_FOUND;
		for (auto const& adapter : adapters)
		{
			hr = D3D11CreateDevice(adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, Flags, FeatureLevels, NumFeatureLevels, SdkVersion, Device, FeatureLevel, ImmediateContext);
			if (SUCCEEDED(hr)) break;
		}
		if (FAILED(hr))
		{
			throw_hresult(hr, "failed to create the DirectX device");
		}
	}
}
