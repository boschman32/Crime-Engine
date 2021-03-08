#pragma once

namespace cr {

	class DX12_Device;
	class DX12_CommandList;

	class DX12_Mesh : public Mesh {
	public:
		DX12_Mesh(MeshBuffer, DX12_Device&, DX12_CommandList*, DirectX::BoundingBox);
		~DX12_Mesh() override;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const;

		void DisposeUploaders();
		DirectX::BoundingBox GetBounds() const { return m_bounds; }
	private:
		DirectX::BoundingBox m_bounds;
		DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R16_UINT;

		Microsoft::WRL::ComPtr<ID3DBlob> m_vertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_indexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBufferUploader = nullptr;
	};
}