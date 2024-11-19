#pragma once

class RootSignature
{
public:
	void Init();

	void CreateGraphicsRootSignature();
	void CreateComputeRootSignature();

	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
	ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return _computeRootSignature; }

private:
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;
	ComPtr<ID3D12RootSignature>	_computeRootSignature;
	D3D12_STATIC_SAMPLER_DESC _samplerDesc{};
};


