#pragma once

namespace cr
{
	class DX12_Device;
	class DX12_CommandList;
	class DX12_Descriptor;

	class DX12_DepthStencilBuffer
	{
	public:
		DX12_DepthStencilBuffer(DX12_Device&, DX12_CommandList&, int, int);
		void SetBuffer(DX12_Device&, DX12_CommandList&, int, int);
		~DX12_DepthStencilBuffer();
		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_dsBuffer;
		DXGI_FORMAT m_dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DX12_Descriptor* m_dsv;
	};
}