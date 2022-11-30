// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "dxgi.hpp"
#include <d3d11.h>

namespace wdul
{
	void d3d11_create_device(
		_Outptr_ ID3D11Device** const Device,
		_Outptr_opt_ ID3D11DeviceContext** const ImmediateContext,
		_In_ IDXGIFactory6* const DxgiFactory,
		std::uint32_t const NumFeatureLevels = 0,
		_In_reads_opt_(NumFeatureLevels) D3D_FEATURE_LEVEL const* const FeatureLevels = nullptr,
		_Out_opt_ D3D_FEATURE_LEVEL* const FeatureLevel = nullptr,
		std::uint32_t const Flags = WDUL_DEBUG_SWITCH(D3D11_CREATE_DEVICE_DEBUG, 0),
		DXGI_GPU_PREFERENCE const GpuPreference = dxgi_default_gpu_preference,
		bool const NoSoftwareAdapter = true,
		std::uint32_t const SdkVersion = D3D11_SDK_VERSION
	);


	template <class DevT, class DcT, std::uint32_t NumFeatureLevels>
	void d3d11_create_device(
		_Outptr_ DevT** const Device,
		_Outptr_ DcT** const ImmediateContext,
		_In_ IDXGIFactory6* const DxgiFactory,
		D3D_FEATURE_LEVEL const (&FeatureLevels)[NumFeatureLevels],
		_Out_opt_ D3D_FEATURE_LEVEL* const FeatureLevel = nullptr,
		std::uint32_t const Flags = WDUL_DEBUG_SWITCH(D3D11_CREATE_DEVICE_DEBUG, 0),
		DXGI_GPU_PREFERENCE const GpuPreference = dxgi_default_gpu_preference,
		bool const NoSoftwareAdapter = true,
		std::uint32_t const SdkVersion = D3D11_SDK_VERSION
	)
	{
		com_ptr<ID3D11Device> device;
		com_ptr<ID3D11DeviceContext> immediateContext;
		d3d11_create_device(
			device.put(),
			immediateContext.put(),
			DxgiFactory,
			NumFeatureLevels, FeatureLevels,
			FeatureLevel,
			Flags,
			GpuPreference,
			NoSoftwareAdapter,
			SdkVersion
		);
		device.as(Device);
		immediateContext.as(ImmediateContext);
	}

	template <class DevT, std::uint32_t NumFeatureLevels>
	void d3d11_create_device(
		_Outptr_ DevT** const Device,
		_In_ IDXGIFactory6* const DxgiFactory,
		D3D_FEATURE_LEVEL const (&FeatureLevels)[NumFeatureLevels],
		_Out_opt_ D3D_FEATURE_LEVEL* const FeatureLevel = nullptr,
		std::uint32_t const Flags = WDUL_DEBUG_SWITCH(D3D11_CREATE_DEVICE_DEBUG, 0),
		DXGI_GPU_PREFERENCE const GpuPreference = dxgi_default_gpu_preference,
		bool const NoSoftwareAdapter = true,
		std::uint32_t const SdkVersion = D3D11_SDK_VERSION
	)
	{
		com_ptr<ID3D11Device> device;
		d3d11_create_device(
			device.put(),
			nullptr,
			DxgiFactory,
			NumFeatureLevels, FeatureLevels,
			FeatureLevel,
			Flags,
			GpuPreference,
			NoSoftwareAdapter,
			SdkVersion
		);
		device.as(Device);
	}
}
