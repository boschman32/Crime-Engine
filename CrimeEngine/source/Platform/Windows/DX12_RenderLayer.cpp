#include "cepch.h"
#include "Platform/Windows/DX12_RenderLayer.h"

#include "Gfx/Camera2D.h"

#include "Platform/Windows/WinWindow.h"
#include "Platform/Windows/DX12_Device.h"
#include "Platform/Windows/DX12_SwapChain.h"
#include "Platform/Windows/DX12_CommandList.h"
#include "Platform/Windows/DX12_CommandQueue.h"
#include "Platform/Windows/DX12_Descriptor.h"
#include "Platform/Windows/DX12_DepthStencilBuffer.h"
#include "Platform/Windows/DX12_UploadBuffer.h"
#include "Platform/Windows/DX12_TextRenderer.h"

#include "Platform/Windows/DX12_RootSignature.h"
#include "Platform/Windows/DX12_PSO.h"

#include "Platform/Windows/DX12_Mesh.h"

#include "Platform/Windows/DX12_Helper.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
#include <DirectXMathMatrix.inl>
#include "Gfx/IDrawable.h"

#include "Platform/Windows/DX12_TextureLoader.h"
#include "Core/ResourceManager.h"
#include "Core/Resource.h"

cr::DX12_RenderLayer::DX12_RenderLayer(Window & a_window)
{
	m_onInit = true;
#if defined(CE_DEBUG)
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	debugInterface->EnableDebugLayer();
#endif

	m_window = &dynamic_cast<WinWindow&>(a_window);

	//Create the required types
	m_device = new DX12_Device();

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	m_cmdQueue = new DX12_CommandQueue(queueDesc, *m_device);
	m_swapChain = new DX12_SwapChain(*m_device, *m_window, *m_cmdQueue);
	m_cmdList = new DX12_CommandList(queueDesc.Type, *m_device, static_cast<int>(m_window->GetSize().x), static_cast<int>(m_window->GetSize().y));

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 3;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	m_rtv = new DX12_Descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, desc, *m_device);
	m_swapChain->CreateRTV(*m_device, *m_rtv);

	//SRVHeap(Descriptor)
	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 1;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDesc.NodeMask = 0;

	m_srv = new DX12_Descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvDesc, *m_device);


	RECT rect;
	::GetClientRect(m_window->GetHWND(), &rect);
	int w = static_cast<int>(rect.right - rect.left);
	int h = static_cast<int>(rect.bottom - rect.top);

	m_dsBuffer = new DX12_DepthStencilBuffer(*m_device, *m_cmdList, w, h);

	m_textureLoader = new DX12_TextureLoader(*m_device, *m_cmdList);
	m_textRenderer = new DX12_TextRenderer(*m_device, *m_cmdList, w, h);

	GeneratePrimitives();

	m_cBuffer = new DX12_UploadBuffer<ConstantBuffer>(*m_device, true);

	InitImGui();

	ID3D12CommandQueue* cmdQueues[] = { m_cmdQueue->GetQueue() };
	OPTICK_GPU_INIT_D3D12(m_device->GetDevice(), cmdQueues, 1);
}

cr::DX12_RenderLayer::~DX12_RenderLayer()
{
	m_cmdQueue->Flush();
	// ImGui shutdown.

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//Clear all data
	delete m_device;
	delete m_swapChain;
	delete m_cmdQueue;
	delete m_cmdList;
	delete m_rtv;
	delete m_srv;
	delete m_dsBuffer;
	delete m_textRenderer;

	for (auto pso : m_pipelineStateObjects)
	{
		delete pso.second;

	}
	m_pipelineStateObjects.clear();

	delete m_textureLoader;
	delete m_cBuffer;

	for (auto m : m_meshes)
	{
		delete m;
	}

	ClearRenderItems();
}

void cr::DX12_RenderLayer::Clear(float a_colour[4])
{
	OPTICK_GPU_EVENT("Clear");

	if (m_onInit)
	{
		m_cmdQueue->ExecuteList(m_cmdList);
		m_cmdQueue->Flush();
		m_onInit = false;
	}
	//Check for resize.
	if (m_window->GetResize())
	{
		Resize();
		m_window->Resize();
	}

	//Clear the FrameBuffer
	m_cColour = a_colour;
	auto commandAllocator = m_cmdList->GetCurrentAllocator(m_swapChain->GetCurrentFrameIndex());
	auto backBuffer = m_swapChain->GetCurrentBuffer();

	commandAllocator->Reset();
	m_cmdList->Reset(m_swapChain->GetCurrentFrameIndex(), m_pipelineStateObjects[m_defaultPSO]);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer,
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	m_cmdList->GetList()->ResourceBarrier(1, &barrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtv->GetHeap()->GetCPUDescriptorHandleForHeapStart(),
		m_swapChain->GetCurrentFrameIndex(), m_rtv->GetSize());

	D3D12_CPU_DESCRIPTOR_HANDLE dvsHandle = m_dsBuffer->DepthStencilView();

	m_cmdList->GetList()->ClearRenderTargetView(rtvHandle, m_cColour, 0, nullptr);
	m_cmdList->GetList()->ClearDepthStencilView(dvsHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_cmdList->GetList()->OMSetRenderTargets(1, &rtvHandle, 1, &dvsHandle);
	// New ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void cr::DX12_RenderLayer::Draw(IDrawable* a_drawable)
{
	OPTICK_EVENT()
	const auto &data = a_drawable->m_drawData;

	if (data.m_canBeCulled && (m_cullObjects && CullObject(data.m_pos)))
	{
		return;
	}

	DrawData* dData = new DrawData(a_drawable->m_drawData);
	m_drawList.push_back(dData);

	m_lastDrawSize++;
}

void cr::DX12_RenderLayer::RenderText(TextObject& a_tObject)
{
	ce::Vector2 pos = { 0,1 };
	pos.x = a_tObject.m_pos.x;
	pos.y -= a_tObject.m_pos.y;

	ce::Vector2 size = a_tObject.m_size;
	ce::Vector2 padding = a_tObject.m_padding;
	ce::Vector4 color = a_tObject.m_color;

	m_textRenderer->RenderText(0, a_tObject.m_text, pos, size, padding, color);
}

void cr::DX12_RenderLayer::PreRender(ce::Camera& a_cam)
{
	OPTICK_GPU_EVENT("Pre Render");

	if (m_window == nullptr)
	{
		delete this; // if there is no window we should delete ourself as we can't exist without a window to draw to
		return;
	}

	float aspect = (a_cam.GetSize().x / a_cam.GetSize().y);
	m_camPos = DirectX::XMFLOAT2(-a_cam.GetPosition().x * aspect, -a_cam.GetPosition().y * aspect);

	ce::Vector2 bounds = dynamic_cast<ce::Camera2D&>(a_cam).GetBounds();
	m_camSize = DirectX::XMFLOAT2((bounds.x), (bounds.y));

	//ViewProjection.
	ConstantBuffer cData = ConstantBuffer();
	DirectX::XMStoreFloat4x4(&cData.m_viewProjectionMat, DirectX::XMMatrixTranspose(DX12_Helper::GlmMat4ToDxMat4(a_cam.GetViewProjectionMatrix())));

	//TODO: Make UI projection?
	//Projection.
	const ce::Vector2& size = a_cam.GetSize();
	DirectX::XMStoreFloat4x4(&cData.m_projectionMat, DirectX::XMMatrixOrthographicLH(size.x, size.y, 0, 100));
	//DirectX::XMStoreFloat4x4(&cData.m_projectionMat, DirectX::XMMatrixTranspose(DX12_Helper::GlmMat4ToDxMat4(a_cam.GetProjectionMatrix())));


	m_cBuffer->CopyData(0, cData);

	for (auto r : m_resourcesToLoad)
	{
		LoadResource(*r.first, *r.second, true);
	}
	m_resourcesToLoad.clear();

	//Sort draw data on shaderID.
	sort(m_drawList.begin(), m_drawList.end(), [](const DrawData* a_lhs, const DrawData* a_rhs)
	{
		return a_lhs->m_shaderId.Value() < a_rhs->m_shaderId.Value();
	});

	GenerateRenderList();
	int instanceItems = 0;

	for (auto& renderable : m_renderables)
	{
		instanceItems += static_cast<int>(sizeof(renderable->m_instances) / sizeof(InstanceBuffer));
		renderable->m_instanceBuffer = new DX12_UploadBuffer<InstanceBuffer>(*m_device, false);
	}

	m_instanceItems = instanceItems;
	InstanceBufferGeneration();
	Logger();
}

void cr::DX12_RenderLayer::GenerateRenderList()
{
	OPTICK_EVENT();

	//BucketSort All Items on Z axis to sort the drawOrder

	for (auto dData : m_drawList)
	{
		RenderItem* rItem = nullptr;

		for (auto rI : m_renderables)
		{
			if (rI->m_instanceCount > (GS_MAX_INSTANCE_SIZE - 1) || rI->m_shaderId != dData->m_shaderId.Value())
			{
				continue;
			}

			if (rI->m_drawType == dData->m_drawtype)
			{
				rItem = rI;
				break;
			}
		}

		if (rItem == nullptr)
		{
			rItem = new RenderItem();

			rItem->m_mesh = m_meshes[static_cast<int>(dData->m_drawtype)];
			rItem->m_drawType = dData->m_drawtype;
			rItem->m_typology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			rItem->m_startIndexLocation = 0;
			rItem->m_baseVertexLocation = 0;
			rItem->m_shaderId = dData->m_shaderId.Value();
			m_renderables.push_back(rItem);
		}

		InstanceBuffer data = InstanceBuffer();

		DirectX::XMFLOAT4X4 identity = DX12_Helper::Identity4x4();

		const DirectX::XMMATRIX p = DirectX::XMMatrixTranslation(dData->m_pos.x,dData->m_pos.y, dData->m_pos.z);
		const DirectX::XMMATRIX s = DirectX::XMMatrixScaling(dData->m_size.x, dData->m_size.y, 1);
		const DirectX::XMMATRIX r = DirectX::XMMatrixRotationZ(dData->m_z);

		const DirectX::XMMATRIX model = r * s * p;

		DirectX::XMStoreFloat4x4(&data.m_modelMat, DirectX::XMMatrixTranspose(model));

		data.m_colour = DX12_Helper::GlmVec4ToDxFloat4(dData->m_color);
		const DXTexture* dxT = m_textureLoader->GetTexture(dData->m_textureId.Value());

		unsigned int tId = 0;

		if (dxT != nullptr)
		{
			ce::Vector4 textureRect = ce::Vector4(0, 0, 1, 1);
			tId = dxT->m_srvOffset;
			const ce::Vector2 textureSize = ce::Vector2(
				static_cast<float>(dxT->m_resource->GetDesc().Width),
				static_cast<float>(dxT->m_resource->GetDesc().Height));

			if (dData->m_rect.x != 0 || dData->m_rect.z != 1)
			{
				textureRect.x = (1 / textureSize.x) * (dData->m_rect.x + 0.5f);
				textureRect.z = (1 / textureSize.x) * (dData->m_rect.z - 0.5f);
			}
			if (dData->m_rect.y != 0 || dData->m_rect.w != 1)
			{
				textureRect.y = (1 / textureSize.y) * (dData->m_rect.y + 0.5f);
				textureRect.w = (1 / textureSize.y) * (dData->m_rect.w - 0.5f);
			}
			data.m_textureCoords = DirectX::XMFLOAT4(DX12_Helper::GlmVec4ToDxFloat4(textureRect));
		}
		data.m_textureId = tId;
		rItem->m_instances[rItem->m_instanceCount] = data;
		rItem->m_instanceCount++;
	}
}

void cr::DX12_RenderLayer::InstanceBufferGeneration()
{
	OPTICK_EVENT();
	for (auto rI : m_renderables)
	{
		//Generate InstanceBuffer with a specific size
		rI->m_instanceBuffer->SetElementCount(*m_device, rI->m_instanceCount, false);

		int item = 0;
		for (auto iData : rI->m_instances)
		{
			if (item >= rI->m_instanceCount)
			{
				break;
			}

			InstanceBuffer instanceData = InstanceBuffer();


			instanceData.m_modelMat = iData.m_modelMat;
			instanceData.m_textureCoords = iData.m_textureCoords;
			instanceData.m_colour = iData.m_colour;
			instanceData.m_textureId = iData.m_textureId;

			int alignmentSize = (rI->m_instanceBuffer->GetBufferAlignment() * item++);
			rI->m_instanceBuffer->CopyData(alignmentSize, instanceData);
		}
	}
}

void cr::DX12_RenderLayer::Render()
{
	OPTICK_GPU_EVENT("Render Frame");
	ImGui::EndFrame();

	//Present the current FrameBuffer
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_textureLoader->GetTextureHeap().GetHeap().Get() };
	m_cmdList->GetList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_cmdList->SetViewPort(1);
	OPTICK_GPU_CONTEXT(m_cmdList->GetList());
	OPTICK_GPU_EVENT("Draw CommandList");
	uint64_t lastId = 0;
	for (auto r : m_renderables)
	{
		if (r->m_shaderId != lastId)
		{
			lastId = r->m_shaderId;
			m_pipelineStateObjects[r->m_shaderId]->BindPSO(m_cmdList);

			//TODO: Find solution.
			//D3D12_CPU_DESCRIPTOR_HANDLE dvsHandle = m_dsBuffer->DepthStencilView();
			//m_cmdList->GetList()->ClearDepthStencilView(dvsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			m_cmdList->GetList()->SetGraphicsRootConstantBufferView(1, m_cBuffer->GetResource().Get()->GetGPUVirtualAddress());
			m_cmdList->GetList()->SetGraphicsRootDescriptorTable(2, m_textureLoader->GetTextureHeap().GetHeap()->GetGPUDescriptorHandleForHeapStart());
		}
		auto buffer = r->m_instanceBuffer;
		m_cmdList->Draw(r, buffer->GetResource().Get());
	}

	m_textRenderer->SetSrv();
	m_textRenderer->DrawAllText(m_dsBuffer->DepthStencilView());

	ID3D12DescriptorHeap* imGuiHeap[] = { m_srv->GetHeap().Get() };
	m_cmdList->GetList()->SetDescriptorHeaps(_countof(imGuiHeap), imGuiHeap);

	// Render ImGui
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_cmdList->GetList());


	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_swapChain->GetCurrentBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	m_cmdList->GetList()->ResourceBarrier(1, &barrier);

	m_cmdQueue->ExecuteList(m_cmdList);

	m_swapChain->Present();

	ClearRenderItems();
}

void cr::DX12_RenderLayer::Logger()
{
	if (m_instanceItems != m_lastFrameInstanceSize)
	{
		CE_GFX_WARN("Need to {0} buffers ", static_cast<int>(ceil(m_instanceItems / GS_MAX_INSTANCE_SIZE)) + 1);
		CE_GFX_INFO("Change size of Drawables size of {0}", m_lastDrawSize);
	}
}

bool cr::DX12_RenderLayer::CullObject(ce::Vector3 a_pos)
{
	DirectX::XMFLOAT2 oPos = DirectX::XMFLOAT2(a_pos.x, a_pos.y);

	if (oPos.y > (m_camPos.y - m_camSize.y) && oPos.y < (m_camPos.y + m_camSize.y))
	{
		return false;
	}

	if (oPos.x > (m_camPos.x - m_camSize.x) && oPos.x <(m_camPos.x + m_camSize.x))
	{
		return false;
	}

	return true;
}

void cr::DX12_RenderLayer::InitImGui()
{
	// Init ImGui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	m_imGuiContext = ImGui::CreateContext();
	m_imGuiIO = &ImGui::GetIO();

	// Enable Docking
	m_imGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Set font
	m_imGuiIO->Fonts->AddFontDefault();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(m_window->GetHWND());
	ImGui_ImplDX12_Init(
		m_device->GetDevice(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		m_srv->GetHeap().Get(),
		m_srv->GetHeap().Get()->GetCPUDescriptorHandleForHeapStart(),
		m_srv->GetHeap().Get()->GetGPUDescriptorHandleForHeapStart()
	);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}

void cr::DX12_RenderLayer::LoadResource(ce::Resource & a_resource, ResourceData& a_data, bool a_canLoad)
{

	OPTICK_EVENT();
	if (!m_onInit && !a_canLoad)
	{
		m_resourcesToLoad.emplace_back(std::pair<ce::Resource*, ResourceData*>(&a_resource,&a_data));
		return;
	}

	if (a_resource.GetType() == ce::ResourceType::Texture)
	{
		m_textureLoader->LoadTexture(a_resource, dynamic_cast<cr::TextureData&>(a_data));
	}
	else if (a_resource.GetType() == ce::ResourceType::Shader)
	{

		OPTICK_EVENT();
		uint64_t id = a_resource.GetID().Value();
		//PSO
		if (m_defaultPSO == 0)
		{
			m_defaultPSO = id;
		}

		m_pipelineStateObjects[id] = std::move(new DX12_PSO(
			a_resource.GetPath(),
			*m_device,
			new DX12_RootSignature(*m_device)));
	}

}

void cr::DX12_RenderLayer::UnloadResource(ce::Resource& a_resource)
{
	OPTICK_EVENT();
	if (a_resource.GetType() == ce::ResourceType::Texture)
	{
		m_textureLoader->UnloadTexture(a_resource);
	}
	else if (a_resource.GetType() == ce::ResourceType::Shader)
	{
		OPTICK_EVENT("Shaders");
		uint64_t id = a_resource.GetID().Value();

		for (auto pso : m_pipelineStateObjects)
		{
			if (pso.first == id)
			{
				CE_GFX_WARN("Shader : {0} is unloaded", a_resource.GetFileName());
				delete pso.second;
				m_pipelineStateObjects.erase(id);
				break;
			}
		}
	}
}

void cr::DX12_RenderLayer::Resize()
{
	RECT rect;
	::GetClientRect(m_window->GetHWND(), &rect);
	int w = static_cast<int>(rect.right - rect.left);
	int h = static_cast<int>(rect.bottom - rect.top);

	w = std::max(w, 1);
	h = std::max(h, 1);

	ImGui_ImplDX12_InvalidateDeviceObjects();
	m_swapChain->ClearRTV();

	m_cBuffer->GetResource().Reset();

	m_swapChain->Resize(w,h);
	m_cmdList->SetViewAndScissor(w, h);
	m_swapChain->CreateRTV(*m_device, *m_rtv);

	m_dsBuffer->SetBuffer(*m_device, *m_cmdList, w, h);
	ImGui_ImplDX12_CreateDeviceObjects();
}

void cr::DX12_RenderLayer::GeneratePrimitives()
{
	MeshBuffer mesh;
	DirectX::BoundingBox bounds;

	// QUAD
	mesh.m_vertices =
	{
		Vertex(-0.5f,  0.5f, 0.0f), // top left
		Vertex(0.5f,  0.5f, 0.0f),  // top right
		Vertex(-0.5f, -0.5f, 0.0f), // bottom left 
		Vertex(0.5f, -0.5f, 0.0f), // bottom right 
	};
	mesh.m_indices =
	{
		0, 1, 3,
		0, 3, 2
	};

	bounds = DX12_Helper::GetBounds(mesh);

	m_meshes.push_back(new DX12_Mesh(mesh, *m_device, m_cmdList, bounds));

// Traingle

	mesh.m_vertices =
	{
		Vertex(0.0f, 0.5f, 0.0f),
		Vertex(0.5f, -0.5f, 0.0f),
		Vertex(-0.5f, -0.5f, 0.0f)
	};
	mesh.m_indices =
	{
		0, 1, 2
	};

	bounds = DX12_Helper::GetBounds(mesh);

	m_meshes.push_back(new DX12_Mesh(mesh, *m_device, m_cmdList, bounds));

}

void cr::DX12_RenderLayer::ClearRenderItems()
{
	m_lastDrawSize = 0;
	m_lastFrameInstanceSize = m_instanceItems;
	for (auto renderable : m_renderables)
	{
		delete renderable->m_instanceBuffer;
		delete renderable;
	}
	
	for (auto dData : m_drawList)
	{
		delete dData;
	}

	m_renderables.clear();
	m_drawList.clear();
}
