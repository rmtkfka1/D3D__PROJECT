#include "pch.h"
#include "Mesh.h"
#include "Core.h"
#include "RootSignature.h"
#include "RenderTargets.h"
#include "D3D12ResourceManager.h"
#include <optional>
Mesh::Mesh():ResourceBase(ResourceType::Mesh)
{

}
Mesh::~Mesh()
{
}
void Mesh::Init(vector<Vertex>& vec , vector<uint32>& index)
{
	CreateVertexBuffer(vec);
	CreateIndexBuffer(index);
}

void Mesh::Init(vector<Vertex>& vec)
{
	CreateVertexBuffer(vec);
}

void Mesh::Render(D3D_PRIMITIVE_TOPOLOGY topolgy)
{
	ComPtr<ID3D12GraphicsCommandList>& cmdlist = core->GetGraphics()->GetCmdLIst();
	cmdlist->IASetPrimitiveTopology(topolgy);
	cmdlist->IASetVertexBuffers(0, 1, &_vertexBufferView);
	cmdlist->IASetIndexBuffer(& _indexBufferView); 
	cmdlist->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}

void Mesh::RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY topolgy)
{
	ComPtr<ID3D12GraphicsCommandList>& cmdlist = core->GetGraphics()->GetCmdLIst();
	cmdlist->IASetPrimitiveTopology(topolgy);
	cmdlist->IASetVertexBuffers(0, 1, &_vertexBufferView); 
	cmdlist->DrawInstanced(_vertexCount, 1, 0, 0);
}

void Mesh::CreateVertexBuffer(vector<Vertex>& vec)
{
	shared_ptr<D3D12ResourceManager>& _resourceManager = core->GetResourceManager();
	ComPtr<ID3D12GraphicsCommandList>& cmdList = _resourceManager->GetCmdList();


	_vertexCount = static_cast<uint32>(vec.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	//DEFAULT 버퍼 생성
	auto hr = core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	ThrowIfFailed(hr);

	//cmdMemory->Reset();
	//cmdList->Reset(cmdMemory.Get(), nullptr);

	//UPLOAD 버퍼 생성
	ID3D12Resource* uploadBuffer = nullptr;

	auto hr2 = core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	ThrowIfFailed(hr2);

	void* data = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(uploadBuffer->Map(0, &readRange, &data));
	::memcpy(data, &vec[0], bufferSize);
	uploadBuffer->Unmap(0, nullptr);

	//복사작업

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	cmdList->CopyBufferRegion(_vertexBuffer.Get(), 0, uploadBuffer, 0, bufferSize);
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	_resourceManager->Excute();

	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex);
	_vertexBufferView.SizeInBytes = bufferSize;

	if (uploadBuffer)
	{
		uploadBuffer->Release();
		uploadBuffer = nullptr;
	}
}

void Mesh::CreateIndexBuffer(vector<uint32>& vec)
{
	shared_ptr<D3D12ResourceManager>& _resourceManager = core->GetResourceManager();
	ComPtr<ID3D12GraphicsCommandList>& cmdList = _resourceManager->GetCmdList();


	_indexCount = static_cast<uint32>(vec.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	//DEFAULT 버퍼 생성
	auto hr = core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&_IndexBuffer));

	ThrowIfFailed(hr);



	//UPLOAD 버퍼 생성
	ID3D12Resource* uploadBuffer = nullptr;

	auto hr2 = core->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	ThrowIfFailed(hr2);

	void* data = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowIfFailed(uploadBuffer->Map(0, &readRange, &data));
	::memcpy(data, &vec[0], bufferSize);
	uploadBuffer->Unmap(0, nullptr);

	//복사작업
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	cmdList->CopyBufferRegion(_IndexBuffer.Get(), 0, uploadBuffer, 0, bufferSize);
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_IndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

	_resourceManager->Excute();

	_indexBufferView.BufferLocation = _IndexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;

	if (uploadBuffer)
	{
		uploadBuffer->Release();
		uploadBuffer = nullptr;
	}

}
