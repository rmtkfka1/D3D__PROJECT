#include "pch.h"
#include "SteamOutputBuffer.h"
#include "Core.h"

void StreamOutputBuffer::Init(uint64 bufferSize)
{

    // 버퍼 설명 생성
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    // Stream Output Buffer 생성
    HRESULT hr = core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_STREAM_OUT, 
        nullptr,
        IID_PPV_ARGS(&_StreamOutputBuffer)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create Stream Output Buffer");
    }

    // Filled Size Buffer 생성
    hr = core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // DEFAULT 타입
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint64)),
        D3D12_RESOURCE_STATE_STREAM_OUT,
        nullptr,
        IID_PPV_ARGS(&_FilledSizeBuffer)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create Filled Size Buffer");
    }

    hr = core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint64)),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&_FilledSizeReadbackBuffer)
    );

    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create Readback Buffer");
    }

    // Stream Output Buffer View 설정
    _SOBufferView.BufferLocation = _StreamOutputBuffer->GetGPUVirtualAddress();
	_SOBufferView.SizeInBytes = bufferSize; //PoSITION 16 + NORMAL 12 + TEXCOORD 8
    _SOBufferView.BufferFilledSizeLocation = _FilledSizeBuffer->GetGPUVirtualAddress();

}

void StreamOutputBuffer::Bind()
{   
	core->GetCmdLIst()->SOSetTargets(0,1,&_SOBufferView);
}

void StreamOutputBuffer::UnBind()
{
	core->GetCmdLIst()->SOSetTargets(0, 0, nullptr);
}

void StreamOutputBuffer::Render()
{
    // 1. Filled Size 데이터를 Readback 버퍼로 복사
    core->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _FilledSizeBuffer.Get(),
        D3D12_RESOURCE_STATE_STREAM_OUT,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    ));

    core->GetCmdLIst()->CopyResource(_FilledSizeReadbackBuffer.Get(), _FilledSizeBuffer.Get());

    core->GetCmdLIst()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _FilledSizeBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_SOURCE,
        D3D12_RESOURCE_STATE_STREAM_OUT
    ));

    // 2. Readback Buffer에서 Filled Size 읽기
    uint32 filledSize = 0;
    CD3DX12_RANGE readRange(0, 0);
    void* data = nullptr;
    HRESULT hr = _FilledSizeReadbackBuffer->Map(0, &readRange, &data);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to map Filled Size Readback Buffer");
    }

    filledSize = *reinterpret_cast<uint32*>(data);
    _FilledSizeReadbackBuffer->Unmap(0, nullptr);

    if (filledSize == 0)
    {
        return;
    }

    D3D12_VERTEX_BUFFER_VIEW VertexView;
    VertexView.BufferLocation = _StreamOutputBuffer->GetGPUVirtualAddress();
    VertexView.StrideInBytes = sizeof(Vertex);
    VertexView.SizeInBytes = filledSize; 

    auto commandList = core->GetCmdLIst();

    commandList->IASetVertexBuffers(0, 1, &VertexView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    uint32 vertexCount = filledSize / sizeof(Vertex);
    commandList->DrawInstanced(vertexCount, 1, 0, 0);


}

