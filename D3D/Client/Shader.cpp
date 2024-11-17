#include "pch.h"
#include "Shader.h"
#include "Core.h"
#include "RootSignature.h"

Shader::Shader():ResourceBase(ResourceType::Shader)
{

}
Shader::~Shader()
{

}

void Shader::Init(const wstring& path, ShaderInfo info )
{

	wstring finalPath = _path + path;
	_info = info;

	CreateVertexShader(finalPath, "VS_Main", "vs_5_0");

	if (info.bActvieStreamOutput == false)
	{
		CreatePixelShader(finalPath, "PS_Main", "ps_5_0");
	}

	if (info.bActiveGSShader)
	{
		CreateGeometryShader(finalPath, "GS_Main", "gs_5_0");
	}


	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	
	_pipelineDesc.InputLayout = { desc, _countof(desc) };
	_pipelineDesc.pRootSignature = core->GetRootSignature()->GetSignature().Get();

	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	_pipelineDesc.SampleMask = UINT_MAX;
	_pipelineDesc.PrimitiveTopologyType = info.primitiveType;
	_pipelineDesc.NumRenderTargets = 1;
	_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	_pipelineDesc.SampleDesc.Count = 1;
	_pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	

	switch (info.shaderType)
	{
	case ShaderType::DEFREED:
		_pipelineDesc.NumRenderTargets = 3;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; //POS
		_pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT; //NORMAL
		_pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; //COLOR
		break;
	default:
		_pipelineDesc.NumRenderTargets = 1;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	switch (info.rasterizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		_pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		_pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (info.depthStencilType)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
		_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::DPTH_TEST_NO_WRITE:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}

	D3D12_RENDER_TARGET_BLEND_DESC& rt = _pipelineDesc.BlendState.RenderTarget[0];

	switch (info.blendType)
	{
	case BLEND_TYPE::DEFAULT:
		rt.BlendEnable = FALSE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ZERO;
		break;
	case BLEND_TYPE::ALPHA_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case BLEND_TYPE::ONE_TO_ONE_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ONE;
		break;
	}

	if (info.bActvieStreamOutput)
	{

		D3D12_SO_DECLARATION_ENTRY entry[] =
		{
			{0, "POSITION", 0, 0, 3, 0},  // Stream 0: POSITION (3 컴포넌트)
			{0, "TEXCOORD", 0, 0, 2, 0},  // Stream 0: TEXCOORD (2 컴포넌트)
			{0, "NORMAL", 0, 0, 3, 0},    // Stream 0: NORMAL (3 컴포넌트)
			{0, "TANGENT", 0, 0, 3, 0}    // Stream 0: TANGENT (3 컴포넌트)
		};

		// 버퍼 스트라이드는 4의 배수여야 하며, 요소들의 크기를 합산한 값이어야 합니다.
		// POSITION(12바이트) + TEXCOORD(8바이트) + NORMAL(12바이트) + TANGENT(12바이트) = 44바이트
		UINT strides[] = { 44 };  // 각 스트림의 스트라이드 값은 44바이트

		// 스트림 출력 설명자
		D3D12_STREAM_OUTPUT_DESC soDesc = {};
		soDesc.pSODeclaration = entry;  // 선언된 스트림 출력 엔트리 배열
		soDesc.NumEntries = _countof(entry);  // 엔트리 수 (4개)
		soDesc.pBufferStrides = strides;  // 스트라이드 배열
		soDesc.NumStrides = _countof(strides); // 스트라이드 개수 (1개)
		soDesc.RasterizedStream = D3D12_SO_NO_RASTERIZED_STREAM;  // 래스터화된 스트림은 없음

		// 파이프라인 설명자에 스트림 출력 설정 적용
		_pipelineDesc.StreamOutput = soDesc;
	}



	core->GetDevice()->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
}


void Shader::SetPipelineState()
{
	core->GetCmdLIst()->SetPipelineState(_pipelineState.Get());
}

void Shader::CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _vsBlob, _pipelineDesc.VS);
}

void Shader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _psBlob, _pipelineDesc.PS);
}

void Shader::CreateGeometryShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _gsBlob, _pipelineDesc.GS);
}
