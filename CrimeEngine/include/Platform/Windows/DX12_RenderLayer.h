#pragma once
#include "Gfx/RenderLayer.h"

#include "DX12_UploadBuffer.h"
#include "Core/Math.h"

struct ImGuiContext;
struct ImGuiIO;

namespace ce
{
	class Camera;
}

namespace cr
{
	class IDrawable;
	class WinWindow;
	class DX12_Device;
	class DX12_SwapChain;
	class DX12_CommandList;
	class DX12_CommandQueue;
	class DX12_Descriptor;
	class DX12_DepthStencilBuffer;
	class DX12_PSO;
	class DX12_RootSignature;
	class DX12_Mesh;
	class DX12_TextureLoader;
	class DX12_TextRenderer;

	struct RenderItem;
	struct DrawData;

	class DX12_RenderLayer : public RenderLayer
	{
	public:
		DX12_RenderLayer(Window&);
		~DX12_RenderLayer() override;

		void Clear(float[4]) override;
		void Draw(IDrawable*) override; // Require a Drawable which contains data : Transform, VertexBuffer, IndexBuffer, Material etc.
		void RenderText(TextObject&) override;
		void PreRender(ce::Camera&) override;
		void Render() override;

		void InitImGui() override;

		void LoadResource(ce::Resource&, ResourceData&, bool = false) override;
		void UnloadResource(ce::Resource&) override;

	private:
		bool m_onInit = false;
		float* m_cColour;
		void Resize();
		void GeneratePrimitives();
		void ClearRenderItems();

		void GenerateRenderList();
		void InstanceBufferGeneration();
		void Logger();
		bool CullObject(ce::Vector3);

		WinWindow* m_window;

		DX12_Device* m_device;
		DX12_SwapChain* m_swapChain;
		DX12_CommandQueue* m_cmdQueue;
		DX12_CommandList* m_cmdList;
		DX12_Descriptor* m_rtv;
		DX12_Descriptor* m_srv;
		DX12_TextRenderer* m_textRenderer;
		DX12_DepthStencilBuffer* m_dsBuffer;

		DX12_UploadBuffer<ConstantBuffer>* m_cBuffer;

		uint64_t m_defaultPSO = 0;
		std::unordered_map<uint64_t, DX12_PSO*> m_pipelineStateObjects;

		std::vector<std::pair<ce::Resource*, ResourceData*>> m_resourcesToLoad;
		DX12_TextureLoader* m_textureLoader;

		std::vector<DrawData*> m_drawList;
		std::vector<DX12_Mesh*> m_meshes;

		std::vector<RenderItem*> m_renderables;

		ImGuiContext* m_imGuiContext;
		ImGuiIO* m_imGuiIO;

		int m_lastFrameInstanceSize = 0;
		int m_instanceItems = 0;
		int m_lastDrawSize = 0;

		DirectX::XMFLOAT2 m_camPos;
		DirectX::XMFLOAT2 m_camSize;
		bool m_cullObjects = true;
	};
}
