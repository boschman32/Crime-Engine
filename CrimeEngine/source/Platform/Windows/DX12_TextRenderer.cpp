#include "cepch.h"

#include "Platform/Windows/DX12_TextRenderer.h"
#include "Platform/Windows/DX12_Device.h"
#include "Platform/Windows/DX12_RootSignature.h"
#include "Platform/Windows/DX12_CommandList.h"

#include <dx12TextureLoading/WICTextureLoader.h>


#include "Core/ResourceManager.h"
#include "Utils/ServiceManager.h"

cr::DX12_TextRenderer::DX12_TextRenderer(DX12_Device& a_device, DX12_CommandList& a_cmd, int a_w, int a_h)
{
	m_device = &a_device;
	m_cmd = &a_cmd;

	GeneratePSO();

	D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
	srvDesc.NumDescriptors = 32;
	srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDesc.NodeMask = 0;

	m_textSrv = new DX12_Descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvDesc, a_device);

	TextItem* calibri = new TextItem();
	const std::string devicePath = ce::ServiceManager::GetService<ce::ResourceManager>()->GetDevicePath("[Assets]fonts/Calibri.fnt");
	calibri->m_font = LoadFontFromFile(devicePath, a_w, a_h);
	CreateTextResource(*calibri->m_font);
	CreateVertexBuffer(*calibri);
	m_textItems.push_back(calibri);
}

cr::DX12_TextRenderer::~DX12_TextRenderer()
{
	m_pso.ReleaseAndGetAddressOf();
	m_signature.ReleaseAndGetAddressOf();
	
	delete m_textSrv;

	for (auto tI : m_textItems)
	{
		delete tI;
	}

	m_textItems.clear();
}

cr::Font* cr::DX12_TextRenderer::LoadFontFromFile(std::string a_fileName, int a_width, int a_height)
{
	std::wifstream fs;
	std::wstring fileName = std::wstring(a_fileName.begin(), a_fileName.end());
	fs.open(fileName);

	float windowWidth = static_cast<float>(a_width);
	float windowHeight = static_cast<float>(a_height);

	Font* font = new Font();
	std::wstring tmp;
	int startpos;

	// extract font name
	fs >> tmp >> tmp; // info face=" XXXX "
	startpos = static_cast<int>(tmp.find(L"")) + 1;
		font->m_fontName = tmp.substr(startpos, tmp.size() - startpos - 1);

	// get font size
	fs >> tmp; // size=73
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

	// get padding
	fs >> tmp; // padding=5,5,5,5 
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

	// get up padding
	startpos = static_cast<int>(tmp.find(L",")) + 1;
	font->m_topPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / windowWidth;

	// get right padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = static_cast<int>(tmp.find(L",")) + 1;
	font->m_rightPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / windowWidth;

	// get down padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = static_cast<int>(tmp.find(L",")) + 1;
	font->m_bottomPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / windowWidth;

	// get left padding
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	font->m_leftPadding = static_cast<float>(std::stoi(tmp)) / windowWidth;

	fs >> tmp; // spacing=0,0

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp >> tmp; // common lineHeight=95
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_lineHeight = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / windowHeight;

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	fs >> tmp; // base=68
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_baseHeight = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / windowHeight;

	// get texture width
	fs >> tmp; // scaleW=512
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_textureWidth = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos)));

	// get texture height
	fs >> tmp; // scaleH=512
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_textureHeight = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos)));

	// get pages, packed, page id
	fs >> tmp >> tmp; // pages=1 packed=0
	fs >> tmp >> tmp; // page id=0

	// get texture filename
	std::wstring wtmp;
	fs >> wtmp; // file="Arial.png"
	startpos = static_cast<int>(wtmp.find(L"\"") + 1);
		font->m_fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

	// get number of characters
	fs >> tmp >> tmp; // chars count=97
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the character list
	font->m_charList = new FontChar[font->m_numCharacters];

	for (int c = 0; c < font->m_numCharacters; ++c)
	{
		// get unicode id
		fs >> tmp >> tmp; // char id=0
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get x
		fs >> tmp; // x=392
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_u = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / static_cast<float>(font->m_textureWidth);

		// get y
		fs >> tmp; // y=340
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_v = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / static_cast<float>(font->m_textureHeight);

		// get width
		fs >> tmp; // width=47
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font->m_charList[c].m_width = static_cast<float>(std::stoi(tmp)) / windowWidth;
		font->m_charList[c].m_tWidth = static_cast<float>(std::stoi(tmp)) / static_cast<float>(font->m_textureWidth);

		// get height
		fs >> tmp; // height=57
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		tmp = tmp.substr(startpos, tmp.size() - startpos);
		font->m_charList[c].m_height = static_cast<float>(std::stoi(tmp)) / windowHeight;
		font->m_charList[c].m_tHeight = static_cast<float>(std::stoi(tmp)) / static_cast<float>(font->m_textureHeight);

		// get xoffset
		fs >> tmp; // xoffset=-6
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_xOffset = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / windowWidth;

		// get yoffset
		fs >> tmp; // yoffset=16
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_yOffset = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / windowHeight;

		// get xadvance
		fs >> tmp; // xadvance=65
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_charList[c].m_xAdvance = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / windowWidth;

		// get page
		// get channel
		fs >> tmp >> tmp; // page=0    chnl=0
	}

	// get number of kernings
	fs >> tmp >> tmp; // kernings count=96
	startpos = static_cast<int>(tmp.find(L"=")) + 1;
	font->m_numKerning = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

	// initialize the kernings list
	font->m_kerningList = new FontKerning[font->m_numKerning];

	for (int k = 0; k < font->m_numKerning; ++k)
	{
		// get first character
		fs >> tmp >> tmp; // kerning first=87
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_kerningList[k].m_firstId = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get second character
		fs >> tmp; // second=45
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		font->m_kerningList[k].m_secondId = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get amount
		fs >> tmp; // amount=-1
		startpos = static_cast<int>(tmp.find(L"=")) + 1;
		int t = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
		font->m_kerningList[k].m_amount = static_cast<float>(t) / windowWidth;
	}

	return font;
}

void cr::DX12_TextRenderer::SetSrv()
{
	ID3D12DescriptorHeap* srvHeap[] = { m_textSrv->GetHeap().Get() };
	m_cmd->GetList()->SetDescriptorHeaps(_countof(srvHeap), srvHeap);

	m_cmd->GetList()->SetGraphicsRootSignature(m_signature.Get());
	m_cmd->GetList()->SetPipelineState(m_pso.Get());
}

void cr::DX12_TextRenderer::DrawAllText(D3D12_CPU_DESCRIPTOR_HANDLE a_dsView)
{
	// clear the depth buffer so we can draw over everything
	m_cmd->GetList()->ClearDepthStencilView(a_dsView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	for (auto tI : m_textItems)
	{
		m_cmd->GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_cmd->GetList()->IASetVertexBuffers(0, 1, &tI->m_textVertexBufferView);
		m_cmd->GetList()->SetGraphicsRootDescriptorTable(1, tI->m_font->m_srvHandle);
		m_cmd->GetList()->DrawInstanced(4, tI->m_numCharacters, 0, 0);

		tI->m_numCharacters = 0;
	}
}

void cr::DX12_TextRenderer::RenderText(unsigned int a_fontId, std::string a_text, ce::Vector2 a_pos, ce::Vector2 a_size = { 1,1 }, ce::Vector2 a_padding = { 0.5f,0 }, ce::Vector4 a_color = {1,1,1,1})
{
	if (m_textItems.empty() || a_fontId > static_cast<unsigned int>(m_textItems.size()))
	{
		return;
	}

	auto textItem = m_textItems[a_fontId];
	auto font = textItem->m_font;
	std::wstring text = std::wstring(a_text.begin(), a_text.end());

	float topLeftScreenX = (a_pos.x * 2.0f) - 1.0f;
	float topLeftScreenY = ((1.0f - a_pos.y) * 2.0f) - 1.0f;

	float x = topLeftScreenX;
	float y = topLeftScreenY;

	float horrizontalPadding = (font->m_leftPadding + font->m_rightPadding) * a_padding.x;
	float verticalPadding = (font->m_topPadding + font->m_bottomPadding) * a_padding.y;

	TextVertex* vert = reinterpret_cast<TextVertex*>(textItem->m_textVBGpuAdress);

	wchar_t lastChar = -1;

	for (int i = 0; i < text.size(); ++i)
	{
		wchar_t c = text[i];

		FontChar* fc = font->GetChar(c);

		if (fc == nullptr)
		{
			continue;
		}
		if (c == L'\0')
		{
			break;
		}
		if (c == L'\n')
		{
			x = topLeftScreenX;
			y -= (font->m_lineHeight + verticalPadding) * a_size.y;
			continue;
		}

		if (textItem->m_numCharacters >= m_maxCharacterPerFrame)
		{
			break;
		}

		float kerning = 0.0f;
		if (i > 0)
		{
			kerning = font->GetKerning(lastChar, c);
		}
		vert[textItem->m_numCharacters] = TextVertex(
			{ 
				x + ((fc->m_xOffset + kerning) * a_size.x) ,
				y - (fc->m_yOffset * a_size.y),
				fc->m_width * a_size.x,
				fc->m_height * a_size.y

			},
			{
				fc->m_u,
				fc->m_v,
				fc->m_tWidth,
				fc->m_tHeight
			},
			a_color
		);

		textItem->m_numCharacters++;
		x += (fc->m_xAdvance - horrizontalPadding) * a_size.x;

		lastChar = c;
	}

}

void cr::DX12_TextRenderer::GeneratePSO()
{
	//------- Signature Root Text -----------
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;


	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; 
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = 1;
	descriptorTableRanges[0].BaseShaderRegister = 0;
	descriptorTableRanges[0].RegisterSpace = 0; 
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; 

	// create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	D3D12_ROOT_PARAMETER  rootParameters[2];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; 

	
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = descriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters),
		rootParameters,
		1,
		&sampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);

	m_device->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_signature));


	/* ----------TEXTPSO----------*/
	std::wstring shaderPath = L"data/shaders/TextShader.hlsl";

	ID3DBlob* errorBuff;
	ID3DBlob* textVertexShader;


	D3DCompileFromFile(shaderPath.c_str(),
		nullptr,
		nullptr,
		"VS",
		"vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&textVertexShader,
		&errorBuff);

	D3D12_SHADER_BYTECODE textVertexShaderBytecode = {};
	textVertexShaderBytecode.BytecodeLength = textVertexShader->GetBufferSize();
	textVertexShaderBytecode.pShaderBytecode = textVertexShader->GetBufferPointer();

	ID3DBlob* textPixelShader;
	D3DCompileFromFile(shaderPath.c_str(),
		nullptr,
		nullptr,
		"PS",
		"ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&textPixelShader,
		&errorBuff);

	D3D12_SHADER_BYTECODE textPixelShaderBytecode = {};
	textPixelShaderBytecode.BytecodeLength = textPixelShader->GetBufferSize();
	textPixelShaderBytecode.pShaderBytecode = textPixelShader->GetBufferPointer();

	D3D12_INPUT_ELEMENT_DESC textInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
	};

	D3D12_INPUT_LAYOUT_DESC textInputLayoutDesc = {};

	textInputLayoutDesc.NumElements = sizeof(textInputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	textInputLayoutDesc.pInputElementDescs = textInputLayout;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC textpsoDesc = {};
	textpsoDesc.InputLayout = textInputLayoutDesc;
	textpsoDesc.pRootSignature = m_signature.Get();
	textpsoDesc.VS = textVertexShaderBytecode;
	textpsoDesc.PS = textPixelShaderBytecode;
	textpsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	textpsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	textpsoDesc.SampleDesc = { 1,0 };
	textpsoDesc.SampleMask = 0xffffffff;
	textpsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	D3D12_BLEND_DESC textBlendStateDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	textBlendStateDesc.AlphaToCoverageEnable = 1;

	textpsoDesc.BlendState = textBlendStateDesc;
	textpsoDesc.NumRenderTargets = 1;
	D3D12_DEPTH_STENCIL_DESC textDepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	textDepthStencilDesc.DepthEnable = 0;
	textpsoDesc.DepthStencilState = textDepthStencilDesc;

	// create the text pso
	m_device->GetDevice()->CreateGraphicsPipelineState(&textpsoDesc, IID_PPV_ARGS(&m_pso));
}

void cr::DX12_TextRenderer::CreateVertexBuffer(TextItem& a_tI)
{
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC  buffer = CD3DX12_RESOURCE_DESC::Buffer(m_maxCharacterPerFrame * sizeof(TextVertex));

	m_device->GetDevice()->CreateCommittedResource(
		&prop, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer, // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&a_tI.m_textVertexBuffer));

	a_tI.m_textVertexBuffer->SetName(L"Text Vertex Buffer Upload Resource Heap");

	CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

	// map the resource heap to get a gpu virtual address to the beginning of the heap
	a_tI.m_textVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&a_tI.m_textVBGpuAdress));


	a_tI.m_textVertexBufferView.BufferLocation = a_tI.m_textVertexBuffer->GetGPUVirtualAddress();
	a_tI.m_textVertexBufferView.StrideInBytes = sizeof(TextVertex);
	a_tI.m_textVertexBufferView.SizeInBytes = m_maxCharacterPerFrame * sizeof(TextVertex);

}

void cr::DX12_TextRenderer::CreateTextResource(Font& a_font)
{
	D3D12_RESOURCE_DESC fontTextureDesc;
	D3D12_SUBRESOURCE_DATA subresources;
	std::unique_ptr<uint8_t[]> txtData;

	std::string fontImage(a_font.m_fontImage.begin(), a_font.m_fontImage.end());

	if (!LoadFontImage(txtData, a_font, fontImage, subresources, fontTextureDesc))
	{
		CE_GFX_ERROR("Can't Load {0} font", fontImage);
		return;
	}
	CD3DX12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	m_device->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&fontTextureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&a_font.m_textureBuffer));
	a_font.m_textureBuffer->SetName(L"Arial Texture Buffer Resource Heap");

	ID3D12Resource* fontTextureBufferUploadHeap;
	UINT64 fontTextureUploadBufferSize;
	m_device->GetDevice()->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize);

	prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize);

	// create an upload heap to copy the texture to the gpu
	m_device->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&fontTextureBufferUploadHeap));

	fontTextureBufferUploadHeap->SetName(L"Font Texture Buffer Upload Resource Heap");

	// Now we copy the upload buffer contents to the default heap
	UpdateSubresources(m_cmd->GetList(), a_font.m_textureBuffer.Get(), fontTextureBufferUploadHeap, 0, 0, 1, &subresources);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(a_font.m_textureBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	m_cmd->GetList()->ResourceBarrier(1, &barrier);

	// create an srv for the font
	D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
	fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	fontsrvDesc.Format = fontTextureDesc.Format;
	fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	fontsrvDesc.Texture2D.MipLevels = 1;

	// we need to get the next descriptor location in the descriptor heap to store this srv
	a_font.m_srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_textSrv->GetHeap()->GetGPUDescriptorHandleForHeapStart(), 1, m_textSrv->GetSize());

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_textSrv->GetHeap()->GetCPUDescriptorHandleForHeapStart(), 1, m_textSrv->GetSize());
	m_device->GetDevice()->CreateShaderResourceView(a_font.m_textureBuffer.Get(), &fontsrvDesc, srvHandle);
}

bool cr::DX12_TextRenderer::LoadFontImage(std::unique_ptr<uint8_t[]>& a_data, Font& a_font, std::string a_path, D3D12_SUBRESOURCE_DATA& a_subData, D3D12_RESOURCE_DESC& a_resource)
{
	const std::string devicePath = ce::ServiceManager::GetService<ce::ResourceManager>()->GetDevicePath("[Assets]fonts/" + a_path);
	const auto path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(devicePath);
	HRESULT hr = S_OK;

	hr = DirectX::LoadWICTextureFromFile(
		m_device->GetDevice(),
		path.c_str(),
		a_font.m_textureBuffer.ReleaseAndGetAddressOf(),
		a_data,
		a_subData);

	if (hr != S_OK)
	{
		return false;
	}


	a_resource = {};
	a_resource.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	a_resource.Alignment = 0;
	a_resource.Width = a_font.m_textureBuffer->GetDesc().Width;
	a_resource.Height = a_font.m_textureBuffer->GetDesc().Height;
	a_resource.DepthOrArraySize = 1; 
	a_resource.MipLevels = 1;
	a_resource.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	a_resource.SampleDesc.Count = 1;
	a_resource.SampleDesc.Quality = 0;
	a_resource.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; 
	a_resource.Flags = D3D12_RESOURCE_FLAG_NONE;

	return true;
}

