// This file is part of the WillDaisey/WDUL (Windows Desktop Utility Library) project.
// View this project on github: https://github.com/WillDaisey/wdul/

#include "include/wdul/d3d12.hpp"
#include "include/wdul/fs.hpp"
#include <limits>

namespace wdul
{
	void d3d12_enable_debug_layer()
	{
		com_ptr<ID3D12Debug> debug;
		check_hresult(D3D12GetDebugInterface(IID_PPV_ARGS(debug.put())), "D3D12GetDebugInterface failed");
		debug->EnableDebugLayer();
	}

	void d3d12_serialize_root_signature(D3D12_VERSIONED_ROOT_SIGNATURE_DESC const& Desc, _Outptr_ ID3DBlob** Blob)
	{
		com_ptr<ID3DBlob> errorBlob;
		auto const hr = D3D12SerializeVersionedRootSignature(&Desc, Blob, errorBlob.put());
		if (FAILED(hr))
		{
			throw_hresult(hr, errorBlob ? static_cast<char const*>(errorBlob->GetBufferPointer()) : std::string_view());
		}
	}
}
