#pragma once

enum class TextureType
{
	Texture2D,
	CubeMap
};

class Texture 
{
public:
	Texture();
	~Texture();

	void Init(const wstring& path,TextureType type = TextureType::Texture2D);


	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

public:



private:
	wstring _path = L"../Resources/Texture/";
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle{};
};

