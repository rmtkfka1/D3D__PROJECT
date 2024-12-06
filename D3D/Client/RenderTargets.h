#pragma once


/*************************
*                        *
*      RenderTargets     *
*                        *
**************************/
class Texture;

class RenderTargets
{

public:
	RenderTargets();
	~RenderTargets();

public:
	void Init(DWORD WndWidth, DWORD WndHeight , ComPtr<IDXGISwapChain3> swapchain);
	void Resize(DWORD BackBufferWidth, DWORD BackBufferHeight, ComPtr<IDXGISwapChain3> swapchain, UINT	_swapChainFlags);

	void RenderBegin();
	void RenderEnd();
	void ClearDepth();
	shared_ptr<Texture>& GetInterMediateTexture() { return _InterMediateTexture; }

	void SetIndex(UINT index) { _RenderTargetIndex = index; }

	//[µ∆€µÂ∑ª¥ı∏µ,∆˜øˆµÂ∑ª¥ı∏µ] -> [ ¡ﬂ∞£≈ÿΩ∫√ƒ ] -> [∑ª¥ı≈∏∞Ÿ]

private:

	shared_ptr<Texture> _RenderTargets[SWAP_CHAIN_FRAME_COUNT];  // [∑ª¥ı≈∏∞Ÿ]
	shared_ptr<Texture> _DSTexture;
	shared_ptr<Texture> _InterMediateTexture; //[ ¡ﬂ∞£≈ÿΩ∫√ƒ ]

	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	DWORD			_Width = 0;
	DWORD			_height = 0;

	uint32	_RenderTargetIndex = 0;

};

/*************************
*                        *
*         GBuffer        *
*                        *
**************************/
class Texture;

class GBuffer
{
public:
	static const uint32 _count = 3; 

	GBuffer();
	~GBuffer();

	void Init();
	void RenderBegin();
	void RenderEnd();

	shared_ptr<Texture>& GetTexture(int32 index);

private:
	array<shared_ptr<Texture>, _count> _textures = { nullptr, };
	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissorRect;
};

class Shadow
{

public:

	Shadow();
	~Shadow();

	void Init();
	void RenderBegin();
	void RenderEnd();

	shared_ptr<Texture>& GetTexture() { return _texture; }

private:

	shared_ptr<Texture>  _texture = nullptr;
	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissorRect;

};


