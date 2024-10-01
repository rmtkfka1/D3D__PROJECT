#pragma once


enum class CBV_REGISTER : uint8
{
	b0=0,
};

enum class SRV_REGISTER : uint8
{
	t0 = 1,

};



/*************************
*                        *
*  ConstantBuffer Pool   *
*                        *
**************************/



class ConstantBufferPool
{
public:
	void Init(CBV_REGISTER reg, uint32 size, uint32 count, bool useCamera);
	void PushData(void* buffer, uint32 size);
	void Clear();

private:
	ComPtr<ID3D12Resource>	_cbvBufferPool;
	uint32					_elementSize = 0;
	uint32					_elementCount = 0;

	BYTE* _mappedBuffer = nullptr;

	ComPtr<ID3D12DescriptorHeap>		_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin = {};
	uint32								_handleIncrementSize = 0;

	uint32					_currentIndex = 0;
	CBV_REGISTER			_reg = {};

	bool _useCamera=false;
};


/*************************
*                        *
*  TextureBuffer  Pool   *
*                        *
**************************/



class TextureBufferPool
{

public:
	void Init(int32 count);
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void AllocDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);


private:
	int32 Alloc();

private:
	vector<uint32> _indexGenerator;
	ComPtr<ID3D12DescriptorHeap>	_srvHeap;

	int32 _heapSize = 0;
	int32 _handleIncrementSize = 0;
	int32 _currentIndex = 0;
};


/*************************
*                        *
*    DescriptorTable     *
*                        *
**************************/


class DescriptorTable
{

public:
	void Init(uint32 count, uint64 CameraMaxCount);

	void Clear();
	void CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle);
	void CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetGraphicsRootDescriptorTable(int num);
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }


private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 reg);

private:
	ComPtr<ID3D12DescriptorHeap> _descHeap;

	uint64                  _cameraMaxCount=0;
	uint64                  _currentCameraIndex=0;

	uint64					_handleSize = 0;
	uint64					_groupSize = 0;
	uint64					_groupCount = 0;
	uint32					_currentGroupIndex = 0;


};











