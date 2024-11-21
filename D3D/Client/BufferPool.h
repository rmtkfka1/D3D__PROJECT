#pragma once


enum class CBV_REGISTER : uint8
{
	b2=0,
	b3=1,
};

enum class SRV_REGISTER : uint8
{
	t0 = 2,
	t1,
	t2
};

enum class UAV_REGISTER : uint8
{
	u0 = 5,
	u1,
	u2
};


/*************************
*                        *
*  ConstantBuffer Pool   *
*                        *
**************************/


class ConstantBufferPool
{
public:
	void Init(CBV_REGISTER reg, uint32 size, uint32 count);
	void PushGraphicsData(void* buffer, uint32 size);
	void PushComputeData(void* buffer, uint32 size);
	void SetData(int index ,void* buffer, uint32 size);

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
};


/*************************
*                        *
*  TextureBuffer  Pool   *
*                        *
**************************/

struct heapStructure
{
	vector<uint32> indexGenator;
	ComPtr<ID3D12DescriptorHeap> heap;

	int32 heapSize = 0;
	int32 handleIncrementSize = 0;
	int32 currentIndex = 0;

};


class TextureBufferPool
{

public:
	void Init(int32 SrvUavCount , int32 RTVCount , int32 DSVCount);

	void FreeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void FreeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void FreeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);


	void AllocSRVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);
	void AllocRTVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);
	void AllocDSVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);

private:

	int32 AllocSRV();
	int32 AllocRTV();
	int32 AllocDSV();

private:
	heapStructure _srvHeap;
	heapStructure _rtvHeap;
	heapStructure _dsvHeap;

};








/*********************************
*                                *
*    GraphicsDescriptorTable     *
*                                *
**********************************/


class GraphicsDescriptorTable
{

public:
	void Init(uint32 count);

	void Clear();
	void CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void SetGraphicsRootDescriptorTable();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }


private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 reg);

private:
	ComPtr<ID3D12DescriptorHeap> _descHeap;

	uint64					_handleSize = 0;
	uint64					_groupSize = 0;
	uint64					_groupCount = 0;
	uint32					_currentGroupIndex = 0;

};

/*********************************
*                                *
*    ComputeDescriptorTable      *
*                                *
**********************************/

class ComputeDescriptorTable
{
public:
	void Init();

	void CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg);
	void CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg);
	void CopyUAV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, UAV_REGISTER reg);

	void SetComputeRootDescriptorTable();
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(SRV_REGISTER reg);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UAV_REGISTER reg);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 reg);

private:
	ComPtr<ID3D12DescriptorHeap> _descHeap;
	uint64						_handleSize = 0;
};









