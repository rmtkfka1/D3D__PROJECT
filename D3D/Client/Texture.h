#pragma once
#include "ResourceBase.h"
enum class TextureType
{
	Texture2D,
	CubeMap
};

enum class TextureUsageFlags : uint32_t 
{
	None = 0,
	RTV = 1 << 0,   // Render Target View
	SRV = 1 << 1,   // Shader Resource View
	UAV = 1 << 2,    // Unordered Access View
	DSV = 1 << 3,    // Unordered Access View
};

inline TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) {
	return static_cast<TextureUsageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool HasFlag(TextureUsageFlags value, TextureUsageFlags flag) {
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

class Texture :public ResourceBase
{
public:
	Texture();
	virtual ~Texture();

	void Init(const wstring& path,TextureType type = TextureType::Texture2D);
	void CreateTexture(DXGI_FORMAT format, uint32 width, uint32 height, TextureUsageFlags usageFlags , bool Jump);
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuHandle() { return _rtvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuHandle() { return _srvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCpuHandle() { return _uavHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuHandle() { return _dsvHandle; }

	ComPtr<ID3D12Resource>& GetResource() { return _resource; }
public:
	//FOR GBUFFER
	void SetSrvHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { _srvHandle = handle; }

private:
	wstring _path = L"../Resources/Texture/";
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_uavHandle{};

	static D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle;
};

