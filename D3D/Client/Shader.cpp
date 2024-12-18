#include "pch.h"
#include "Shader.h"
#include "Core.h"
#include "RootSignature.h"
#include <iostream>
Shader::Shader() :ResourceBase(ResourceType::Shader)
{


}

Shader::~Shader()
{
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


GraphicsShader::GraphicsShader() 
{

}
GraphicsShader::~GraphicsShader()
{

} 

void GraphicsShader::Init(const wstring& path, ShaderInfo info)
{

	wstring finalPath = _path + path;
	_info = info;

	CreateVertexShader(finalPath, "VS_Main", "vs_5_0");

	if (info.bActivePixelShader)
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
		{ "BlendIndices", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BlendWeights", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	_pipelineDesc.InputLayout = { desc, _countof(desc) };
	_pipelineDesc.pRootSignature = core->GetRootSignature()->GetGraphicsRootSignature().Get();
	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	_pipelineDesc.SampleMask = UINT_MAX;
	_pipelineDesc.PrimitiveTopologyType = info.primitiveType;
	_pipelineDesc.NumRenderTargets = 1;
	_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	_pipelineDesc.SampleDesc.Count = 1;
	_pipelineDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	switch (info.shaderType)
	{
	case ShaderType::DEFREED:
		_pipelineDesc.NumRenderTargets = 3;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; //POS
		_pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT; //NORMAL
		_pipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; //COLOR
		break;
	case ShaderType::SHADOW:
		_pipelineDesc.NumRenderTargets = 0;
		_pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
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
	case RASTERIZER_TYPE::COUNTERCLOCKWIES:
		_pipelineDesc.RasterizerState.FrontCounterClockwise = true;
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
	case DEPTH_STENCIL_TYPE::DEPTH_TEST_NO_WRITE:
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::STENCILL_WRITE:
		_pipelineDesc.DepthStencilState.StencilEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		_pipelineDesc.DepthStencilState.StencilWriteMask = 0xFF;
		_pipelineDesc.DepthStencilState.StencilReadMask = 0xFF;
		_pipelineDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		_pipelineDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		_pipelineDesc.DepthStencilState.FrontFace.StencilFailOp  = D3D12_STENCIL_OP_KEEP;
		_pipelineDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
		_pipelineDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
		_pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0;
		break;
	case DEPTH_STENCIL_TYPE::STENCILL_READ:
		_pipelineDesc.DepthStencilState.StencilEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthEnable = TRUE;
		_pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		_pipelineDesc.DepthStencilState.StencilWriteMask = 0xFF;
		_pipelineDesc.DepthStencilState.StencilReadMask = 0xFF;
		_pipelineDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		_pipelineDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		_pipelineDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		_pipelineDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		_pipelineDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
		_pipelineDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_ZERO;
		_pipelineDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_ZERO;
		_pipelineDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_ZERO;
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
		rt.LogicOpEnable = D3D12_LOGIC_OP_NOOP;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;

	case BLEND_TYPE::ONE_TO_ONE_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ONE;
		break;

	case BLEND_TYPE::BLEND_FACTOR:
		rt.BlendEnable = TRUE;  // 블렌드 활성화
		rt.LogicOpEnable = FALSE;  // 논리 연산 사용 안 함
		rt.SrcBlend = D3D12_BLEND_BLEND_FACTOR; 
		rt.DestBlend = D3D12_BLEND_INV_BLEND_FACTOR; 
		rt.BlendOp = D3D12_BLEND_OP_ADD;  // 블렌드 연산은 더하기
		rt.RenderTargetWriteMask =D3D12_COLOR_WRITE_ENABLE_ALL;
		break;
	}


	


	if (info.bActvieStreamOutput)
	{

		vector<D3D12_SO_DECLARATION_ENTRY> entry = {
		{0, "POSITION", 0, 0, 3, 0},  // Stream 0: POSITION (3 컴포넌트)
		{0, "TEXCOORD", 0, 0, 2, 0},  // Stream 0: TEXCOORD (2 컴포넌트)
		{0, "NORMAL", 0, 0, 3, 0},    // Stream 0: NORMAL (3 컴포넌트)
		{0, "TANGENT", 0, 0, 3, 0}    // Stream 0: TANGENT (3 컴포넌트)
		};

		UINT strides[] = { 44 };  // 각 스트림의 스트라이드 값은 44바이트

		// 스트림 출력 설명자
		D3D12_STREAM_OUTPUT_DESC soDesc = {};
		soDesc.pSODeclaration = entry.data();  // 선언된 스트림 출력 엔트리 배열
		soDesc.NumEntries = entry.size();  // 엔트리 수 (4개)
		soDesc.pBufferStrides = strides;  // 스트라이드 배열
		soDesc.NumStrides = _countof(strides); // 스트라이드 개수 (1개)
		soDesc.RasterizedStream = D3D12_SO_NO_RASTERIZED_STREAM;  // 래스터화된 스트림은 없음
		_pipelineDesc.StreamOutput = soDesc;

		core->GetDevice()->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
		return;
	}


	core->GetDevice()->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
}


void GraphicsShader::SetPipelineState()
{
	core->GetCmdList()->SetPipelineState(_pipelineState.Get());
}



void GraphicsShader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _vsBlob, _pipelineDesc.VS);
}

void GraphicsShader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _psBlob, _pipelineDesc.PS);
}

void GraphicsShader::CreateGeometryShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _gsBlob, _pipelineDesc.GS);
}

//ComputeShader
ComputeShader::ComputeShader() 
{

}

ComputeShader::~ComputeShader()
{
}

void ComputeShader::Init(const wstring& path)
{
	wstring finalPath = _path + path;

	CreateShader(finalPath, "CS_Main", "cs_5_0", _csBlob, _pipelineDesc.CS);

	_pipelineDesc.pRootSignature = core->GetRootSignature()->GetGraphicsRootSignature().Get();

	HRESULT hr = core->GetDevice()->CreateComputePipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
	assert(SUCCEEDED(hr));
}

void ComputeShader::SetPipelineState()
{
	core->GetCmdList()->SetPipelineState(_pipelineState.Get());
}


