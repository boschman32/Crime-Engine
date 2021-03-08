#include "cepch.h"

#include "Platform/Windows/DX12_TextureLoader.h"

#include "Platform/Windows/DX12_Device.h"
#include "Platform/Windows/DX12_CommandList.h"
#include "Platform/Windows/DX12_Descriptor.h"
#include "Core/Resource.h"

#include "dx12/dx12TextureLoading/DDSTextureLoader.h"
#include "dx12/dx12TextureLoading/WICTextureLoader.h"

#include <regex>

cr::DX12_TextureLoader::DX12_TextureLoader(DX12_Device& a_device, DX12_CommandList& a_cmd)
{
	m_device = &a_device;
	m_cmd = &a_cmd;
	//SRVHeap(Descriptor)
	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 256;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDesc.NodeMask = 0;

	m_textureSRV = new DX12_Descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvDesc, a_device);
}

cr::DX12_TextureLoader::~DX12_TextureLoader()
{
	for (auto t : m_textures)
	{
		delete t.second;
	}

	m_textures.clear();
	delete m_textureSRV;
}

void cr::DX12_TextureLoader::LoadTexture(ce::Resource& a_resource, cr::TextureData& a_textureData)
{
	OPTICK_EVENT("Texture", Optick::Category::Rendering);
	if (!a_resource.m_isReloaded)
	{
		ReloadTexture(a_resource, a_textureData);
		a_resource.m_isReloaded = true;
		return;
	}

	auto newTexture = CreateDXTexture(a_resource);

	if (newTexture == nullptr)
	{
		return;
	}

	LoadToSRV(newTexture);
	CE_GFX_INFO("Succesfully to loaded Texture : {0}", a_resource.GetFileName());

	a_textureData.m_width = static_cast<int>(newTexture->m_resource->GetDesc().Width);
	a_textureData.m_height = static_cast<int>(newTexture->m_resource->GetDesc().Height);
	a_textureData.m_textureId = newTexture->m_srvOffset;

	m_textures[a_resource.GetID().Value()] = std::move(newTexture);
}

void cr::DX12_TextureLoader::UnloadTexture(ce::Resource& a_resource)
{
	OPTICK_EVENT("Texture");
	for (auto t : m_textures)
	{
		if (static_cast<uint64_t>(t.first) == a_resource.GetID().Value())
		{
			m_textures.erase(t.first);
			delete t.second;
			CE_GFX_WARN("Texture : {0} is unloaded", a_resource.GetFileName());
			break;			
		}
	}

}

const cr::DXTexture* cr::DX12_TextureLoader::GetTexture(uint64_t a_id)
{
	if (m_lastRequested.first == a_id)
	{
		return m_lastRequested.second;
	}

	for (auto t : m_textures)
	{
		if (t.first == a_id)
		{
			m_lastRequested.first = a_id;
			m_lastRequested.second = t.second;
			return t.second;
		}
	}

	return nullptr;
}

void cr::DX12_TextureLoader::ReloadTexture(ce::Resource& a_resource, cr::TextureData & a_data)
{
	CE_GFX_WARN("Texture {0} Should be reloaded!", a_resource.GetFileName());


	auto t = m_textures[a_resource.GetID().Value()];
	delete t;
	m_textures.erase(a_resource.GetID().Value());

	DXTexture* newTexture = CreateDXTexture(a_resource);

	LoadToSRV(newTexture);

	a_data.m_width = static_cast<int>(newTexture->m_resource->GetDesc().Width);
	a_data.m_height = static_cast<int>(newTexture->m_resource->GetDesc().Height);
	a_data.m_textureId = newTexture->m_srvOffset;

	m_textures[a_resource.GetID().Value()] = std::move(newTexture);
}

cr::DXTexture * cr::DX12_TextureLoader::CreateDXTexture(ce::Resource& a_resource)
{
	DXTexture* newTexture = new DXTexture();

	std::string fileExtention = FindExtension(a_resource.GetPath());
	auto path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(a_resource.GetPath());
	newTexture->m_resourceId = static_cast<unsigned int>(a_resource.GetID().Value());
	newTexture->m_filename = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(a_resource.GetFileName());

	HRESULT hr = S_OK;
	std::unique_ptr<uint8_t[]> txtData;

	if (fileExtention == "dds")
	{
		hr = DirectX::CreateDDSTextureFromFile12(
			m_device->GetDevice(),
			m_cmd->GetList(),
			path.c_str(),
			newTexture->m_resource,
			newTexture->m_uploadHeap);

		if (hr != S_OK)
		{
			CE_GFX_ERROR("Failed to load Texture : {0}", a_resource.GetFileName());
			delete newTexture;
			return nullptr;
		}
	}
	else if (
		static_cast<int>(fileExtention == "bpm") != 0 ||
		static_cast<int>(fileExtention == "png") != 0 ||
		static_cast<int>(fileExtention == "jpg") != 0)
	{
		D3D12_SUBRESOURCE_DATA subresources;

		hr = DirectX::LoadWICTextureFromFile(
			m_device->GetDevice(),
			path.c_str(),
			newTexture->m_resource.ReleaseAndGetAddressOf(),
			txtData,
			subresources);

		if (hr != S_OK)
		{
			CE_GFX_ERROR("Failed to load Texture : {0}", a_resource.GetFileName());
			delete newTexture;
			return nullptr;
		}

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(newTexture->m_resource.Get(), 0, 1);
		CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		// Create the GPU upload buffer.		
		m_device->GetDevice()->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(newTexture->m_uploadHeap.GetAddressOf()));

		UpdateSubresources(m_cmd->GetList(),
			newTexture->m_resource.Get(),
			newTexture->m_uploadHeap.Get(), 0, 0, 1, &subresources);

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(newTexture->m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		m_cmd->GetList()->ResourceBarrier(1, &barrier);
	}
	else {
		CE_CORE_ASSERT_M(false, "We do not support Textures of the following type : {0}", fileExtention);
	}

	GetOffset(&newTexture->m_srvOffset);

	return newTexture;
}

void cr::DX12_TextureLoader::LoadToSRV(DXTexture* a_texture)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_textureSRV->GetHeap()->GetCPUDescriptorHandleForHeapStart());

	if (a_texture->m_srvOffset != 0)
	{
		hDescriptor.Offset(a_texture->m_srvOffset, m_textureSRV->GetSize());
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	auto tResource = a_texture->m_resource;

	srvDesc.Texture2D.MipLevels = tResource->GetDesc().MipLevels;
	srvDesc.Format = tResource->GetDesc().Format;

	m_device->GetDevice()->CreateShaderResourceView(tResource.Get(), &srvDesc, hDescriptor);
}

bool cr::DX12_TextureLoader::GetOffset(unsigned int * a_srvOffset)
{
	unsigned int newOffset = 0;
	if (!m_textures.empty()) {
		std::map <unsigned int, uint64_t> mappedTextures;

		for (auto t : m_textures)
		{
			mappedTextures[t.second->m_srvOffset] = std::move(t.first);
		}
		for (auto id : mappedTextures)
		{
			if (newOffset != id.first)
			{
				break;
			}
			newOffset++;
		}
	}

	*a_srvOffset = newOffset;
	return true;
}

std::string cr::DX12_TextureLoader::FindExtension(const std::string & a_filename)
{
	std::string returnValue;

	auto position = a_filename.find_last_of('.');
	if (position == std::string::npos)
	{
		returnValue = "";
	}
	else
	{
		std::string extension(a_filename.substr(position + 1));
		if (std::regex_search(extension, std::regex("[^A-Za-z0-9]")))
		{
			returnValue = "";
		}
		else
		{
			returnValue = extension;
		}
	}

	return returnValue;
}
