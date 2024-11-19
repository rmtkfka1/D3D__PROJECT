
#include "pch.h"
#include "Texture.h"
#include "Core.h"
#include "BufferPool.h"
#include "D3D12ResourceManager.h"

Texture::Texture():ResourceBase(ResourceType::Texture)
{

}

Texture::~Texture()
{
	
}

void Texture::Init(const wstring& path,TextureType type)
{
 
    wstring finalPath = _path + path;

    wstring ext = fs::path(finalPath).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(finalPath.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(finalPath.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(finalPath.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    HRESULT hr = ::CreateTexture(core->GetDevice().Get(), _image.GetMetadata(), &_resource);

    if (FAILED(hr))
        assert(nullptr);

    vector<D3D12_SUBRESOURCE_DATA> subResources;

    hr = ::PrepareUpload(core->GetDevice().Get(),
        _image.GetImages(),
        _image.GetImageCount(),
        _image.GetMetadata(),
        subResources);

    if (FAILED(hr))
        assert(nullptr);

    const uint64 bufferSize = ::GetRequiredIntermediateSize(_resource.Get(), 0, static_cast<uint32>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;
    hr = core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

    if (FAILED(hr))
        assert(nullptr);

    hr = core->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            _image.GetMetadata().format,
            _image.GetMetadata().width,
            _image.GetMetadata().height,
            _image.GetMetadata().arraySize,
            _image.GetMetadata().mipLevels),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_resource));

    if (FAILED(hr))
        assert(nullptr);

    auto ResourceManager = core->GetResourceManager();

    auto list = ResourceManager->GetCmdList();
    auto queue = ResourceManager->GetCmdQueue();
    auto memory = ResourceManager->GetCmdMemory();

    memory->Reset();
    list->Reset(memory.Get(), nullptr);

    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

    ::UpdateSubresources(list.Get(),
        _resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());

    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
    list->Close();

    ID3D12CommandList* cmdListArr[] = { list.Get() };
    queue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    ResourceManager->Fence();
    ResourceManager->WaitForFenceValue();

    core->GetBufferManager()->GetTextureBufferPool()->AllocDescriptorHandle(&_srvHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = _image.GetMetadata().format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = _image.GetMetadata().mipLevels;

    switch (type)
    {
    case TextureType::Texture2D:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        break;
    case TextureType::CubeMap:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        break;
    default:
        break;
    }


    core->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}



