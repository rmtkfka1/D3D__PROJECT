#include "pch.h"
#include "SteamOutputBuffer.h"
#include "Core.h"

void StreamOutputBuffer::Init(uint64 bufferSize)
{
    // 滚欺 汲疙 积己
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    // Stream Output Buffer 积己
    ThrowIfFailed(core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_STREAM_OUT,
        nullptr,
        IID_PPV_ARGS(&_StreamOutputBuffer)));


    // Filled Size Buffer 积己
    ThrowIfFailed(core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // DEFAULT 鸥涝
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint64)),
        D3D12_RESOURCE_STATE_STREAM_OUT,
        nullptr,
        IID_PPV_ARGS(&_FilledSizeBuffer)));
 

    ThrowIfFailed(core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint64)),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&_FilledSizeReadbackBuffer)));

    _SOBufferView.BufferLocation = _StreamOutputBuffer->GetGPUVirtualAddress();
	_SOBufferView.SizeInBytes = bufferSize; 
    _SOBufferView.BufferFilledSizeLocation = _FilledSizeBuffer->GetGPUVirtualAddress();

}

void StreamOutputBuffer::Bind()
{   
    core->GetGraphics()->GetCmdLIst()->SOSetTargets(0,1,&_SOBufferView);
}

void StreamOutputBuffer::UnBind()
{
    core->GetGraphics()->GetCmdLIst()->SOSetTargets(0, 0, nullptr);
}

void StreamOutputBuffer::Render()
{
    
    core->GetGraphics()->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _FilledSizeBuffer.Get(),
        D3D12_RESOURCE_STATE_STREAM_OUT,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    ));

    core->GetGraphics()->GetCmdLIst()->CopyResource(_FilledSizeReadbackBuffer.Get(), _FilledSizeBuffer.Get());

    core->GetGraphics()->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _FilledSizeBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        D3D12_RESOURCE_STATE_STREAM_OUT
    ));


    uint32 filledSize = 0;
    CD3DX12_RANGE readRange(0, 0);
    void* data = nullptr;
    ThrowIfFailed(_FilledSizeReadbackBuffer->Map(0, &readRange, &data));
  
    filledSize = *reinterpret_cast<uint32*>(data);
    _FilledSizeReadbackBuffer->Unmap(0, nullptr);

    if (filledSize == 0)
    {
        return;
    }

    D3D12_VERTEX_BUFFER_VIEW VertexView;
    VertexView.BufferLocation = _StreamOutputBuffer->GetGPUVirtualAddress();
    VertexView.StrideInBytes =44;
    VertexView.SizeInBytes = filledSize; 

    auto commandList = core->GetGraphics()->GetCmdLIst();

    commandList->IASetVertexBuffers(0, 1, &VertexView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint32 vertexCount = filledSize / 44;
    commandList->DrawInstanced(vertexCount, 1, 0, 0);


}

