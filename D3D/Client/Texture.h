#pragma once
#include "ResourceBase.h"
enum class TextureType
{
	Texture2D,
	CubeMap
};

class Texture :public ResourceBase
{
public:
	Texture();
	virtual ~Texture();

	void Init(const wstring& path,TextureType type = TextureType::Texture2D);


	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

public:
	//FOR GBUFFER
	void SetSrvHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { _srvHandle = handle; }

private:
	wstring _path = L"../Resources/Texture/";
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle{};
};

