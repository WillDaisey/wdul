// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#pragma once
#include "dxgi.hpp"
#include <d3d12.h>

namespace wdul
{
	void d3d12_enable_debug_layer();

	inline void d3d12_offset_descriptor_handle(
		D3D12_CPU_DESCRIPTOR_HANDLE& CpuDescriptorHandle,
		std::uint32_t const DescriptorIncrementSize,
		std::int64_t const OffsetInDescriptors = 1
	) noexcept
	{
		CpuDescriptorHandle.ptr = static_cast<std::size_t>(static_cast<std::int64_t>(CpuDescriptorHandle.ptr) + (OffsetInDescriptors * static_cast<std::int64_t>(DescriptorIncrementSize)));
	}

	inline void d3d12_offset_descriptor_handle(
		D3D12_GPU_DESCRIPTOR_HANDLE& GpuDescriptorHandle,
		std::uint32_t const DescriptorIncrementSize,
		std::int64_t const OffsetInDescriptors = 1
	) noexcept
	{
		GpuDescriptorHandle.ptr = static_cast<std::uint64_t>(static_cast<std::int64_t>(GpuDescriptorHandle.ptr) + (OffsetInDescriptors * static_cast<std::int64_t>(DescriptorIncrementSize)));
	}

	inline D3D12_RENDER_TARGET_BLEND_DESC constexpr d3d12_default_render_target_blend_desc =
	{
		.BlendEnable = false,
		.LogicOpEnable = false,
		.SrcBlend = D3D12_BLEND_ONE,
		.DestBlend = D3D12_BLEND_ZERO,
		.BlendOp = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha = D3D12_BLEND_ONE,
		.DestBlendAlpha = D3D12_BLEND_ZERO,
		.BlendOpAlpha = D3D12_BLEND_OP_ADD,
		.LogicOp = D3D12_LOGIC_OP_NOOP,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	};

	inline D3D12_BLEND_DESC constexpr d3d12_default_blend_desc =
	{
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = d3d12_default_render_target_blend_desc
	};

	inline D3D12_RASTERIZER_DESC constexpr d3d12_default_rasterizer_desc =
	{
		.FillMode = D3D12_FILL_MODE_SOLID,
		.CullMode = D3D12_CULL_MODE_BACK,
		.FrontCounterClockwise = false,
		.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
		.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		.DepthClipEnable = true,
		.MultisampleEnable = false,
		.AntialiasedLineEnable = false,
		.ForcedSampleCount = 0,
		.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	[[nodiscard]] inline D3D12_INPUT_ELEMENT_DESC d3d12_make_input_element_desc_per_instance(
		_In_z_ char const* const SemanticName,
		std::uint32_t const SemanticIndex,
		DXGI_FORMAT const Format,
		std::uint32_t const InputSlot,
		std::uint32_t const AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
		std::uint32_t const InstanceDataStepRate = 1
	) noexcept
	{
		return {
			.SemanticName = SemanticName,
			.SemanticIndex = SemanticIndex,
			.Format = Format,
			.InputSlot = InputSlot,
			.AlignedByteOffset = AlignedByteOffset,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
			.InstanceDataStepRate = InstanceDataStepRate
		};
	}

	[[nodiscard]] inline D3D12_INPUT_ELEMENT_DESC d3d12_make_input_element_desc_per_vertex(
		_In_z_ char const* const SemanticName,
		std::uint32_t const SemanticIndex,
		DXGI_FORMAT const Format,
		std::uint32_t const InputSlot,
		std::uint32_t const AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT
	) noexcept
	{
		return {
			.SemanticName = SemanticName,
			.SemanticIndex = SemanticIndex,
			.Format = Format,
			.InputSlot = InputSlot,
			.AlignedByteOffset = AlignedByteOffset,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};
	}

	[[nodiscard]] inline D3D12_VIEWPORT d3d12_make_viewport(
		float const TopLeftX,
		float const TopLeftY,
		float const Width,
		float const Height,
		float const MinDepth = D3D12_MIN_DEPTH,
		float const MaxDepth = D3D12_MAX_DEPTH
	) noexcept
	{
		return { .TopLeftX = TopLeftX, .TopLeftY = TopLeftY, .Width = Width, .Height = Height, .MinDepth = MinDepth, .MaxDepth = MaxDepth };
	}

	[[nodiscard]] inline D3D12_VERSIONED_ROOT_SIGNATURE_DESC d3d12_make_root_signature_desc_1_0(
		std::uint32_t const NumParams,
		_In_reads_opt_(NumParams) D3D12_ROOT_PARAMETER const* const Params,
		std::uint32_t const NumStaticSamplers = 0,
		_In_reads_opt_(NumStaticSamplers) D3D12_STATIC_SAMPLER_DESC const* const StaticSamplers = nullptr,
		D3D12_ROOT_SIGNATURE_FLAGS const Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
	) noexcept
	{
		return { .Version = D3D_ROOT_SIGNATURE_VERSION_1_0, .Desc_1_0 = {.NumParameters = NumParams, .pParameters = Params, .NumStaticSamplers = NumStaticSamplers, .pStaticSamplers = StaticSamplers, .Flags = Flags} };
	}

	[[nodiscard]] inline D3D12_VERSIONED_ROOT_SIGNATURE_DESC d3d12_make_root_signature_desc_1_1(
		std::uint32_t const NumParams,
		_In_reads_opt_(NumParams) D3D12_ROOT_PARAMETER1 const* const Params,
		std::uint32_t const NumStaticSamplers = 0,
		_In_reads_opt_(NumStaticSamplers) D3D12_STATIC_SAMPLER_DESC const* const StaticSamplers = nullptr,
		D3D12_ROOT_SIGNATURE_FLAGS const Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
	) noexcept
	{
		return { .Version = D3D_ROOT_SIGNATURE_VERSION_1_1, .Desc_1_1 = {.NumParameters = NumParams, .pParameters = Params, .NumStaticSamplers = NumStaticSamplers, .pStaticSamplers = StaticSamplers, .Flags = Flags} };
	}

	[[nodiscard]] inline D3D12_RESOURCE_BARRIER d3d12_make_resource_barrier_transition(
		ID3D12Resource* const Resource,
		D3D12_RESOURCE_STATES const StateBefore,
		D3D12_RESOURCE_STATES const StateAfter,
		std::uint32_t const Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
		D3D12_RESOURCE_BARRIER_FLAGS const Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE
	) noexcept
	{
		return { .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = Flags, .Transition = {.pResource = Resource, .Subresource = Subresource, .StateBefore = StateBefore, .StateAfter = StateAfter} };
	}

	[[nodiscard]] inline D3D12_RESOURCE_BARRIER d3d12_make_resource_barrier_uav(
		ID3D12Resource* const Resource,
		D3D12_RESOURCE_BARRIER_FLAGS const Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE
	) noexcept
	{
		return { .Type = D3D12_RESOURCE_BARRIER_TYPE_UAV, .Flags = Flags, .UAV = {.pResource = Resource} };
	}

	[[nodiscard]] inline D3D12_RESOURCE_BARRIER d3d12_make_resource_barrier_aliasing(
		ID3D12Resource* const ResourceBefore,
		ID3D12Resource* const ResourceAfter,
		D3D12_RESOURCE_BARRIER_FLAGS const Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE
	) noexcept
	{
		return { .Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING, .Flags = Flags, .Aliasing = {.pResourceBefore = ResourceBefore, .pResourceAfter = ResourceAfter} };
	}

	[[nodiscard]] inline D3D12_HEAP_PROPERTIES d3d12_make_heap_properties(
		D3D12_CPU_PAGE_PROPERTY const CpuPageProperty,
		D3D12_MEMORY_POOL const MemoryPoolPreference,
		std::uint32_t const CreationNodeMask = 0,
		std::uint32_t const NodeMask = 0
	) noexcept
	{
		return { .Type = D3D12_HEAP_TYPE_CUSTOM, .CPUPageProperty = CpuPageProperty, .MemoryPoolPreference = MemoryPoolPreference, .CreationNodeMask = CreationNodeMask, .VisibleNodeMask = NodeMask };
	}

	[[nodiscard]] inline D3D12_HEAP_PROPERTIES d3d12_make_heap_properties(
		D3D12_HEAP_TYPE const Type,
		std::uint32_t const CreationNodeMask = 0,
		std::uint32_t const NodeMask = 0
	) noexcept
	{
		return { .Type = Type, .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN, .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN, .CreationNodeMask = CreationNodeMask, .VisibleNodeMask = NodeMask };
	}

	inline bool d3d12_is_heap_cpu_accessible(D3D12_HEAP_TYPE const Type, D3D12_CPU_PAGE_PROPERTY const CpuPageProperty) noexcept
	{
		return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
			(CpuPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CpuPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
	}

	inline bool d3d12_is_heap_cpu_accessible(D3D12_HEAP_PROPERTIES const& Properties) noexcept
	{
		return d3d12_is_heap_cpu_accessible(Properties.Type, Properties.CPUPageProperty);
	}

	[[nodiscard]] inline D3D12_HEAP_DESC d3d12_make_heap_desc(
		std::uint64_t const Size,
		D3D12_HEAP_PROPERTIES const& Properties,
		std::uint64_t const Alignment = 0,
		D3D12_HEAP_FLAGS const Flags = D3D12_HEAP_FLAG_NONE
	) noexcept
	{
		return { .SizeInBytes = Size, .Properties = Properties, .Alignment = Alignment, .Flags = Flags };
	}

	[[nodiscard]] inline D3D12_HEAP_DESC d3d12_make_heap_desc(
		D3D12_RESOURCE_ALLOCATION_INFO const& ResourceAllocationInfo,
		D3D12_HEAP_PROPERTIES const& Properties,
		D3D12_HEAP_FLAGS const Flags = D3D12_HEAP_FLAG_NONE
	) noexcept
	{
		return { .SizeInBytes = ResourceAllocationInfo.SizeInBytes, .Properties = Properties, .Alignment = ResourceAllocationInfo.Alignment, .Flags = Flags };
	}

	[[nodiscard]] inline D3D12_RESOURCE_DESC d3d12_make_buffer_resource_desc(
		std::uint64_t const Width,
		D3D12_RESOURCE_FLAGS const Flags = D3D12_RESOURCE_FLAG_NONE,
		std::uint64_t const Alignment = 0
	) noexcept
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = Alignment;
		desc.Width = Width;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = Flags;
		return desc;
	}

	[[nodiscard]] inline D3D12_RESOURCE_DESC d3d12_make_buffer_resource_desc(
		D3D12_RESOURCE_ALLOCATION_INFO const& AllocationInfo,
		D3D12_RESOURCE_FLAGS const Flags = D3D12_RESOURCE_FLAG_NONE
	) noexcept
	{
		return d3d12_make_buffer_resource_desc(AllocationInfo.SizeInBytes, Flags, AllocationInfo.Alignment);
	}

	[[nodiscard]] inline D3D12_RESOURCE_DESC d3d12_make_texture1d_resource_desc(
		DXGI_FORMAT const Format,
		std::uint64_t const Width,
		std::uint16_t const ArraySize = 1,
		std::uint16_t const MipLevels = 0,
		D3D12_RESOURCE_FLAGS const Flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT const Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		std::uint64_t const Alignment = 0
	) noexcept
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		desc.Alignment = Alignment;
		desc.Width = Width;
		desc.Height = 1;
		desc.DepthOrArraySize = ArraySize;
		desc.MipLevels = MipLevels;
		desc.Format = Format;
		desc.SampleDesc.Count = 1;
		desc.Layout = Layout;
		desc.Flags = Flags;
		return desc;
	}

	[[nodiscard]] inline D3D12_RESOURCE_DESC d3d12_make_texture2d_resource_desc(
		DXGI_FORMAT const Format,
		std::uint64_t const Width,
		std::uint32_t const Height,
		std::uint16_t const ArraySize = 1,
		std::uint16_t const MipLevels = 0,
		std::uint32_t const SampleCount = 1,
		std::uint32_t const SampleQuality = 0,
		D3D12_RESOURCE_FLAGS const Flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT const Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		std::uint64_t const Alignment = 0
	) noexcept
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = Alignment;
		desc.Width = Width;
		desc.Height = Height;
		desc.DepthOrArraySize = ArraySize;
		desc.MipLevels = MipLevels;
		desc.Format = Format;
		desc.SampleDesc.Count = SampleCount;
		desc.SampleDesc.Quality = SampleQuality;
		desc.Layout = Layout;
		desc.Flags = Flags;
		return desc;
	}

	[[nodiscard]] inline D3D12_RESOURCE_DESC d3d12_make_texture3d_resource_desc(
		DXGI_FORMAT const Format,
		std::uint64_t const Width,
		std::uint32_t const Height,
		std::uint16_t const Depth,
		std::uint16_t const MipLevels = 0,
		D3D12_RESOURCE_FLAGS const Flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT const Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		std::uint64_t const Alignment = 0
	) noexcept
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = Alignment;
		desc.Width = Width;
		desc.Height = Height;
		desc.DepthOrArraySize = Depth;
		desc.MipLevels = MipLevels;
		desc.Format = Format;
		desc.SampleDesc.Count = 1;
		desc.Layout = Layout;
		desc.Flags = Flags;
		return desc;
	}

	void d3d12_serialize_root_signature(D3D12_VERSIONED_ROOT_SIGNATURE_DESC const& Desc, _Outptr_ ID3DBlob** Blob);
}
