#include "pch.h"
#include "RootSignature.h"
#include "Core.h"
#include "BufferPool.h"
void RootSignature::Init()
{
	// ���� ���÷� ����
	D3D12_STATIC_SAMPLER_DESC samplerDesc[2] = {};
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].MipLODBias = 0;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[0].MinLOD = 0.0f;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].MipLODBias = 0;
	samplerDesc[1].MaxAnisotropy = 0;
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[1].MinLOD = 0.0f;
	samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[1].ShaderRegister = 1;
	samplerDesc[1].RegisterSpace = 0;
	samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 2), //b2,b3
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0), //t0,t1,t2
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 3, 0), // u0,u1,u2
	};





	CD3DX12_ROOT_PARAMETER param[4];
	param[0].InitAsConstantBufferView(0);  //������ b0
	param[1].InitAsConstantBufferView(1);  //ī�޶� b1
	param[2].InitAsConstantBufferView(4);  //�ִϸ��̼� b4
	param[3].InitAsDescriptorTable(_countof(ranges), ranges);

	// ��Ʈ ���� ����
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(param);
	rootSignatureDesc.pParameters = param;
	rootSignatureDesc.NumStaticSamplers = 2;
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;


	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	core->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));

}

