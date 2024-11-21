#include "pch.h"
#include "RootSignature.h"
#include "Core.h"
#include "BufferPool.h"
void RootSignature::Init()
{
	CreateGraphicsRootSignature();
	CreateComputeRootSignature();
}

void RootSignature::CreateGraphicsRootSignature()
{
	// 정적 샘플러 설정
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 2),//b2,b3
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0) //t0,t1,t2
	};


	CD3DX12_ROOT_PARAMETER param[3];
	param[0].InitAsConstantBufferView(0);  //라이팅 b0
	param[1].InitAsConstantBufferView(1);  //카메라 b1
	param[2].InitAsDescriptorTable(_countof(ranges), ranges);

	// 루트 서명 설정
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(param);
	rootSignatureDesc.pParameters = param;
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;


	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	core->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));
}

void RootSignature::CreateComputeRootSignature()
{

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 2), //b2,b3
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0), //t0,t1,t2
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 3, 0), // u0,u1,u2
	};


	CD3DX12_ROOT_PARAMETER param[1];
	param[0].InitAsDescriptorTable(_countof(ranges), ranges);


	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(param);
	rootSignatureDesc.pParameters = param;
	rootSignatureDesc.NumStaticSamplers = 1;
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	core->GetDevice()->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_computeRootSignature));

}
