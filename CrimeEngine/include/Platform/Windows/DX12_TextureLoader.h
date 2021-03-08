#pragma once

#include "Platform/Windows/DX12_Helper.h"

namespace ce 
{
	class Resource;
}

namespace cr 
{
	class DX12_Device;
	class DX12_CommandList;
	class DX12_Descriptor;

	class DX12_TextureLoader
	{
	public:
		DX12_TextureLoader(DX12_Device&, DX12_CommandList&);
		~DX12_TextureLoader();

		void LoadTexture(ce::Resource&, cr::TextureData&);
		void UnloadTexture(ce::Resource&);

		const DXTexture* GetTexture(uint64_t);
		inline DX12_Descriptor& GetTextureHeap() { return *m_textureSRV; }
	private:
		void ReloadTexture(ce::Resource&, cr::TextureData&);
		DXTexture* CreateDXTexture(ce::Resource&);
		void LoadToSRV(DXTexture*);
		bool GetOffset(unsigned int*);
		std::string FindExtension(const std::string&);

		DX12_Device* m_device;
		DX12_CommandList* m_cmd;
		DX12_Descriptor* m_textureSRV;

		std::pair<uint64_t, DXTexture*> m_lastRequested;

		std::unordered_map<uint64_t, DXTexture*> m_textures;
	};

}