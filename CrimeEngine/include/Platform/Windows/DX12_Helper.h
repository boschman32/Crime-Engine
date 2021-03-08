#pragma once
#include "Gfx/GFXHelper.h"
#include "DX12_UploadBuffer.h"

namespace cr 
{
	class DX12_Helper
	{
	public:
		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(ID3D12Device*,
			ID3D12GraphicsCommandList*,
			const void*,
			UINT64,
			Microsoft::WRL::ComPtr<ID3D12Resource>&);

		static inline float GetInfinity()
		{
			return FLT_MAX;
		}

		static inline DirectX::BoundingBox GetBounds(MeshBuffer& a_buffer)
		{
			DirectX::BoundingBox bounds;
			float inf = DX12_Helper::GetInfinity();
			DirectX::XMFLOAT3 vMinf3(+inf, +inf, +inf);
			DirectX::XMFLOAT3 vMaxf3(-inf, -inf, -inf);

			DirectX::XMVECTOR vMin = DirectX::XMLoadFloat3(&vMinf3);
			DirectX::XMVECTOR vMax = DirectX::XMLoadFloat3(&vMaxf3);

			for (auto v : a_buffer.m_vertices)
			{
				DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(v.m_pos.x, v.m_pos.y, v.m_pos.z);
				DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pos);
				vMin = DirectX::XMVectorMin(vMin, p);
				vMax = DirectX::XMVectorMax(vMax, p);
			}

			DirectX::XMFLOAT3 center;
			DirectX::XMStoreFloat3(&center, DirectX::XMVectorAdd(vMin, vMax));

			center.x *= 0.5f;
			center.y *= 0.5f;
			center.z *= 0.5f;

			DirectX::XMFLOAT3 extent;
			DirectX::XMStoreFloat3(&extent, DirectX::XMVectorSubtract(vMin, vMax));

			extent.x *= 0.5f;
			extent.y *= 0.5f;
			extent.z *= 0.5f;

			bounds.Center = center;
			bounds.Extents = extent;

			return bounds;
		}

		static inline DirectX::XMFLOAT4X4 Identity4x4()
		{
			static DirectX::XMFLOAT4X4 s_i(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return s_i;
		}

		//Convert glm::mat4 to DirectX::XMMATRIX.
		static inline DirectX::XMMATRIX GlmMat4ToDxMat4(const glm::mat4& a_mat4)
		{
			return DirectX::XMMATRIX(
				a_mat4[0][0], a_mat4[0][1], a_mat4[0][2], a_mat4[0][3],
				a_mat4[1][0], a_mat4[1][1], a_mat4[1][2], a_mat4[1][3],
				a_mat4[2][0], a_mat4[2][1], a_mat4[2][2], a_mat4[2][3],
				a_mat4[3][0], a_mat4[3][1], a_mat4[3][2], a_mat4[3][3]
			);
		}

		//Convert glm::vec4 to DirectX::XMFLOAT4.
		static inline DirectX::XMFLOAT4 GlmVec4ToDxFloat4(const glm::vec4& a_vec4)
		{
			return DirectX::XMFLOAT4(
				a_vec4.r,
				a_vec4.g,
				a_vec4.b,
				a_vec4.a
			);
		}
	};

	static const int GS_MAX_INSTANCE_SIZE = 4096;

	struct DXTexture
	{
		unsigned int m_resourceId;
		unsigned int m_srvOffset = 0;

		std::wstring m_filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadHeap = nullptr;
	};

	//Buffer data from big to small.
	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 m_viewProjectionMat = DX12_Helper::Identity4x4();
		DirectX::XMFLOAT4X4 m_projectionMat = DX12_Helper::Identity4x4();
	};

	struct InstanceBuffer
	{
		DirectX::XMFLOAT4X4 m_modelMat = DX12_Helper::Identity4x4();
		DirectX::XMFLOAT4 m_textureCoords = DirectX::XMFLOAT4(1,1,1,1);
		DirectX::XMFLOAT4 m_colour = DirectX::XMFLOAT4(1, 0, 1, 1);
		unsigned int m_textureId = 0;
	};

	class DX12_Mesh;

	struct RenderItem 
	{
		RenderItem() = default;
		~RenderItem() = default;

		DX12_Mesh* m_mesh = nullptr;
		DrawType m_drawType;

		D3D12_PRIMITIVE_TOPOLOGY m_typology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		// DrawIndexedInstanced parameters.
		unsigned int m_startIndexLocation = 0;
		int m_baseVertexLocation = 0;
		uint64_t m_shaderId;

		DX12_UploadBuffer<InstanceBuffer>* m_instanceBuffer;
		InstanceBuffer m_instances [GS_MAX_INSTANCE_SIZE];
		int m_instanceCount = 0;
		int m_bufferId = 0;
	};
}