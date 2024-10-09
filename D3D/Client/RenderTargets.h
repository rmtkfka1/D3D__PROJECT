#pragma once


/*************************
*                        *
*      RenderTargets     *
*                        *
**************************/

class RenderTargets
{

public:
	RenderTargets();
	~RenderTargets();

public:
	void Init(DWORD WndWidth, DWORD WndHeight , ComPtr<IDXGISwapChain3> swapchain);
	void Resize(DWORD BackBufferWidth, DWORD BackBufferHeight, ComPtr<IDXGISwapChain3> swapchain, UINT	_swapChainFlags);

	void CreateRenderTarget(DWORD WndWidth, DWORD WndHeight, ComPtr<IDXGISwapChain3> swapchain);
	void CreateDepthStencil();

	void RenderBegin();
	void RenderEnd();

	void SetIndex(UINT index) { _RenderTargetIndex = index; }

	ComPtr<ID3D12DescriptorHeap> GetDSVHeap() { return _DSVHeap; }

private:

	ComPtr<ID3D12Resource> _RenderTargets[SWAP_CHAIN_FRAME_COUNT] = {};
	ComPtr<ID3D12Resource> _depthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> _RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _DSVHeap = nullptr;

	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	DWORD			_Width = 0;
	DWORD			_height = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_FRAME_COUNT];
	D3D12_CPU_DESCRIPTOR_HANDLE     _dsvHandle;
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

	void Init(ComPtr<ID3D12DescriptorHeap> DSVHeap);

	void RenderBegin();
	void RenderEnd();

	shared_ptr<Texture> GetTexture(int32 index);


private:
	ComPtr<ID3D12DescriptorHeap> _RTVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _SRVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> _DsvHeap = nullptr; //RENDER TARGET 에서 생성하고 받아올예정

	ComPtr<ID3D12Resource> _resources[_count];  //rtv srv 둘다사용될것임

	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[_count];
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle[_count];
	D3D12_CPU_DESCRIPTOR_HANDLE     _dsvHandle;

	//오브젝트에게 건내주기위해사용
	vector<shared_ptr<Texture>> _textrues;

	D3D12_VIEWPORT _vp;
	D3D12_RECT _rect;
};



