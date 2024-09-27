#pragma once

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetSignature() { return _signature; }

private:
	ComPtr<ID3D12RootSignature>	_signature;
	D3D12_STATIC_SAMPLER_DESC _samplerDesc{};
};


