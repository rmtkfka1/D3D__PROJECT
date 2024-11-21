#include "pch.h"
#include "BufferPool.h"
#include "Core.h"
#include "BufferManager.h"
void ConstantBufferPool::Init(CBV_REGISTER reg, uint32 size, uint32 count)
{
	_reg = reg;
	_elementSize = (size + 255) & ~255;
	_elementCount = count;

	//Buffer Pool 생성
	uint64 bufferSize = _elementSize * _elementCount;

	core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBufferPool));

	_cbvBufferPool->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));


	//힙생성
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
	cbvDesc.NumDescriptors = _elementCount;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	core->GetDevice()->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&_cbvHeap));

	_cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
	_handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (uint32 i = 0; i < _elementCount; ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, i * _handleIncrementSize);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = _cbvBufferPool->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
		cbvDesc.SizeInBytes = _elementSize;
		core->GetDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);
	}

}

void ConstantBufferPool::PushGraphicsData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, _currentIndex * _handleIncrementSize);

	core->GetBufferManager()->GetGraphicsTableHeap()->CopyCBV(cpuHandle, _reg);

	_currentIndex++;
}

void ConstantBufferPool::PushComputeData(void* buffer, uint32 size)
{
	assert(_currentIndex < _elementCount);

	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, _currentIndex * _handleIncrementSize);

	core->GetBufferManager()->GetComputeTableHeap()->CopyCBV(cpuHandle, _reg);

	_currentIndex++;

}

void ConstantBufferPool::SetData(int index ,void* buffer, uint32 size)
{

	assert(_elementSize == ((size + 255) & ~255));

	if (index == 0) //라이팅에 연산될것이므로 한번셋팅하고 건들지않는다.
	{
		::memcpy(&_mappedBuffer[0], buffer, size);
		core->GetGraphics()->GetCmdLIst()->SetGraphicsRootConstantBufferView(0, _cbvBufferPool->GetGPUVirtualAddress());
	}

	else if(index ==1) //카메라 계산에 연산될것이므로 오프셋 계산필요함.
	{
		::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBufferPool->GetGPUVirtualAddress();
		objCBAddress += _currentIndex * _elementSize;
		core->GetGraphics()->GetCmdLIst()->SetGraphicsRootConstantBufferView(1, objCBAddress);
		_currentIndex++;
	}

	else
	{
		assert(false);
	}
}

void ConstantBufferPool::Clear()
{
	_currentIndex = 0;
}


/*************************
*                        *
*  TextureBuffer Pool    *
*                        *
**************************/


void TextureBufferPool::Init(int32 SrvUavCount, int32 RTVCount ,int32 DSVCount)
{
	{
		_srvHeap.heapSize = SrvUavCount;
		_srvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		_srvHeap.indexGenator.resize(_srvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = SrvUavCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap.heap)));
	}

	{
		_rtvHeap.heapSize = RTVCount;
		_rtvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_rtvHeap.indexGenator.resize(_rtvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
		RTVHeapDesc.NumDescriptors = RTVCount;
		RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&_rtvHeap.heap)));
	}

	{
		_dsvHeap.heapSize = DSVCount;
		_dsvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		_dsvHeap.indexGenator.resize(_dsvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = DSVCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_dsvHeap.heap)));

	}

}

void TextureBufferPool::FreeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _srvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _srvHeap.handleIncrementSize;

	assert(index >= 0);

	_srvHeap.indexGenator[index] = -1;
	_srvHeap.currentIndex--;
}

void TextureBufferPool::FreeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _rtvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _rtvHeap.handleIncrementSize;

	assert(index >= 0);

	_rtvHeap.indexGenator[index] = -1;
	_rtvHeap.currentIndex--;

}

void TextureBufferPool::FreeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _dsvHeap.handleIncrementSize;

	assert(index >= 0);

	_dsvHeap.indexGenator[index] = -1;
	_dsvHeap.currentIndex--;
}

void TextureBufferPool::AllocSRVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_srvHeap.currentIndex < _srvHeap.heapSize);

	int32 index = AllocSRV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_srvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _srvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_srvHeap.currentIndex++;

}

void TextureBufferPool::AllocRTVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_rtvHeap.currentIndex < _rtvHeap.heapSize);

	int32 index = AllocRTV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_rtvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _rtvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_rtvHeap.currentIndex++;
}

void TextureBufferPool::AllocDSVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_dsvHeap.currentIndex < _dsvHeap.heapSize);

	int32 index = AllocDSV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_dsvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _dsvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_dsvHeap.currentIndex++;

}



int32 TextureBufferPool::AllocSRV()
{
	for (int i = 0; i < _srvHeap.indexGenator.size(); ++i)
	{
		if (_srvHeap.indexGenator[i] == -1)
		{
			_srvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocRTV()
{
	for (int i = 0; i < _rtvHeap.indexGenator.size(); ++i)
	{
		if (_rtvHeap.indexGenator[i] == -1)
		{
			_rtvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocDSV()
{
	for (int i = 0; i < _dsvHeap.indexGenator.size(); ++i)
	{
		if (_dsvHeap.indexGenator[i] == -1)
		{
			_dsvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}


///////////////////////////////////////////////////////////




/*************************
*                        *
*    DescriptorTable     *
*                        *
**************************/
void GraphicsDescriptorTable::Init(uint32 count)
{
	_groupCount = count;

	int32 RegisterCount = 5;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = count * RegisterCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	core->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	_groupSize = _handleSize * (RegisterCount);

}

void GraphicsDescriptorTable::Clear()
{
	_currentGroupIndex = 0;
}

void GraphicsDescriptorTable::CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorTable::CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicsDescriptorTable::SetGraphicsRootDescriptorTable()
{

	D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	core->GetGraphics()->GetCmdLIst()->SetGraphicsRootDescriptorTable(2, handle);
	_currentGroupIndex++;
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorTable::GetCPUHandle(CBV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorTable::GetCPUHandle(SRV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE GraphicsDescriptorTable::GetCPUHandle(uint32 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += _currentGroupIndex * _groupSize;
	handle.ptr += reg * _handleSize;
	return handle;
}

/// <summary>
/// ///////////////////////
/// </summary>
void ComputeDescriptorTable::Init()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 50;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	core->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

	_handleSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorTable::CopyCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorTable::CopySRV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, SRV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, srcHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorTable::CopyUAV(D3D12_CPU_DESCRIPTOR_HANDLE uavHandle, UAV_REGISTER reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCPUHandle(reg);

	core->GetDevice()->CopyDescriptorsSimple(1, destHandle, uavHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ComputeDescriptorTable::SetComputeRootDescriptorTable()
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
	COMPUTE->_cmdList->SetComputeRootDescriptorTable(0, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorTable::GetCPUHandle(CBV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorTable::GetCPUHandle(SRV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorTable::GetCPUHandle(UAV_REGISTER reg)
{
	return GetCPUHandle(static_cast<uint32>(reg));
}

D3D12_CPU_DESCRIPTOR_HANDLE ComputeDescriptorTable::GetCPUHandle(uint32 reg)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += reg * _handleSize;
	return handle;
}



