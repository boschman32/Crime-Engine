#include "cepch.h"
#include "Platform/Windows/DX12_PSO.h"

#include "Platform/Windows/DX12_Device.h"
#include "Platform/Windows/DX12_RootSignature.h"
#include "Platform/Windows/DX12_CommandList.h"

cr::DX12_PSO::DX12_PSO(std::string a_shader, DX12_Device& a_device, DX12_RootSignature* a_signature)
{
	m_vertShader = a_shader;
	m_pixShader = a_shader;
	m_root = a_signature;

	std::wstring shaderLoc = std::wstring(a_shader.begin(), a_shader.end());

	CreateVertexShader(shaderLoc.c_str());
	CreatePixelShader(shaderLoc.c_str());

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_root->GetSiganture().Get();
	psoDesc.VS = 
	{
		reinterpret_cast<BYTE*>(m_shaders[ShaderType::VertexShader]->GetBufferPointer()),
		m_shaders[ShaderType::VertexShader]->GetBufferSize()
	};
	psoDesc.PS = 
	{
		reinterpret_cast<BYTE*>(m_shaders[ShaderType::FragmentShader]->GetBufferPointer()),
		m_shaders[ShaderType::FragmentShader]->GetBufferSize()
	};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	CD3DX12_RASTERIZER_DESC rDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	psoDesc.RasterizerState = rDesc;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.BlendState = blendDesc;
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	a_device.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
}

cr::DX12_PSO::~DX12_PSO()
{
	m_pso.ReleaseAndGetAddressOf();
	for (auto s : m_shaders)
	{
		s.second.ReleaseAndGetAddressOf();
	}
	delete m_root;
}

void cr::DX12_PSO::BindPSO(DX12_CommandList* a_cmd)
{
	a_cmd->GetList()->SetGraphicsRootSignature(m_root->GetSiganture().Get());
	a_cmd->GetList()->SetPipelineState(m_pso.Get());
}

void cr::DX12_PSO::CreateVertexShader(const wchar_t* a_adress)
{
	D3D12_SHADER_BYTECODE vs = {};

	ID3DBlob* vertexShader;
	ID3DBlob* errorBuff;
	D3DCompileFromFile(a_adress,
		nullptr,
		nullptr,
		"VS",
		"vs_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&errorBuff);

	vs.BytecodeLength = vertexShader->GetBufferSize();
	vs.pShaderBytecode = vertexShader->GetBufferPointer();

	m_shaders[ShaderType::VertexShader] = vertexShader;
}

void cr::DX12_PSO::CreatePixelShader(const wchar_t* a_adress)
{
	D3D12_SHADER_BYTECODE ps = {};

	ID3DBlob* pixelShader;
	ID3DBlob* errorBuff;
	D3DCompileFromFile(a_adress,
		nullptr,
		nullptr,
		"PS",
		"ps_5_1",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&errorBuff);

	ps.BytecodeLength = pixelShader->GetBufferSize();
	ps.BytecodeLength = pixelShader->GetBufferSize();
	ps.pShaderBytecode = pixelShader->GetBufferPointer();

	m_shaders[ShaderType::FragmentShader] = pixelShader;
}
