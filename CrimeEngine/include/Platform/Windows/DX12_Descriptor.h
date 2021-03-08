#pragma once

namespace cr {

	class DX12_Device;

	class DX12_Descriptor {
	public:
		DX12_Descriptor(D3D12_DESCRIPTOR_HEAP_TYPE, D3D12_DESCRIPTOR_HEAP_DESC, cr::DX12_Device&);
		~DX12_Descriptor();

		inline unsigned int GetSize() const { return m_heapSize; }
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetHeap() { return m_heap; }
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
		unsigned int m_heapSize;
	};
}