#pragma once


class Texture 
{
public:
	Texture();
	~Texture();

	void Init(const wstring& path);
	void InitCubeMap(const wstring& path);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

public:



private:
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle{};
};

