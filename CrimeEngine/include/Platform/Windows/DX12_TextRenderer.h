#pragma once
#include "Core/Math.h"

namespace cr
{
	class DX12_CommandList;
	class DX12_Descriptor;
	class DX12_Device;

	struct Font
	{
		std::wstring m_fontName;
		std::wstring m_fontImage;

		int m_size;

		float m_lineHeight;
		float m_baseHeight;

		float m_textureWidth;
		float m_textureHeight;

		int m_numCharacters;
		FontChar* m_charList;

		int m_numKerning;
		FontKerning* m_kerningList;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_textureBuffer;
		D3D12_GPU_DESCRIPTOR_HANDLE m_srvHandle;

		float m_leftPadding;
		float m_topPadding;
		float m_rightPadding;
		float m_bottomPadding;

		FontChar* GetChar(wchar_t a_char)
		{
			for (int i = 0; i < m_numCharacters; ++i)
			{
				if (a_char == static_cast<wchar_t>(m_charList[i].m_id))
				{
					return &m_charList[i];
				}
			}
			return nullptr;
		}

		float GetKerning(wchar_t a_first, wchar_t a_second)
		{
			for (int i = 0; i < m_numKerning; ++i)
			{
				if (static_cast<wchar_t>(m_kerningList[i].m_firstId) == a_first &&
					static_cast<wchar_t>(m_kerningList[i].m_secondId) == a_second)
				{
					return m_kerningList[i].m_amount;
				}
			}

			return 0.0f;
		}
	};

	struct TextItem
	{
		~TextItem()
		{
			m_textVertexBuffer.ReleaseAndGetAddressOf();
			delete m_font->m_charList;
			delete m_font->m_kerningList;
			delete m_font;
		}
		Font* m_font;

		int m_numCharacters = 0;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_textVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_textVertexBufferView;
		UINT8* m_textVBGpuAdress;
	};

	class DX12_TextRenderer
	{
	public :
		DX12_TextRenderer(DX12_Device&, DX12_CommandList&, int, int);
		~DX12_TextRenderer();


		void SetSrv();
		void RenderText(unsigned int, std::string, ce::Vector2, ce::Vector2, ce::Vector2, ce::Vector4);
		void DrawAllText(D3D12_CPU_DESCRIPTOR_HANDLE);
	private:
		Font* LoadFontFromFile(std::string, int, int);
		void GeneratePSO();
		void CreateVertexBuffer(TextItem&);
		void CreateTextResource(Font&);
		bool LoadFontImage(std::unique_ptr<uint8_t[]>&, Font&, std::string, D3D12_SUBRESOURCE_DATA&, D3D12_RESOURCE_DESC&);

		DX12_Descriptor* m_textSrv;
		DX12_CommandList* m_cmd;
		DX12_Device* m_device;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_signature;

		std::vector<TextItem*> m_textItems;
		const int m_maxCharacterPerFrame = 2048;
	};
}