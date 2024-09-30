#include "pch.h"
#include "Terrain.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Core.h"
#include "BufferPool.h"
#include "Shader.h"
#include "Texture.h"
/////////////////////////////////////////////////////////

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
    if (_HeightMapPixels)
    {
        delete[] _HeightMapPixels;
    }
}

void Terrain::Init()
{
    LoadFIle(L"../Resources/Texture/heightMap/heightMap.raw", vec3(10.0f, 3.0f, 10.0f));
    CreateMesh();

    ShaderInfo info;
  
    shared_ptr<Shader> shader = make_shared<Shader>();
    shader->Init(L"default.hlsl", info);

    shared_ptr<Texture> texture = make_shared<Texture>();
    texture->Init(L"heightMap/Base_Texture.dds");

    _material->SetShader(shader);
    _material->SetDiffuseTexture(texture);
}

void Terrain::Update()
{
    _transform->Update();
}

void Terrain::Render()
{
    _transform->PushData();
    _material->Pushdata();
    core->GetTableHeap()->SetGraphicsRootDescriptorTable();

    ComPtr<ID3D12GraphicsCommandList>& cmdlist = core->GetCmdLIst();
    cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdlist->IASetVertexBuffers(0, 1, &_mesh->GetBufferView()); // Slot: (0~15)
    cmdlist->IASetIndexBuffer(&_mesh->GetIndexBufferView()); // Slot: (0~15)
    cmdlist->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
}

void Terrain::CreateMesh()
{

  
    int size = 1;

    vector<Vertex> v;

    // _length�� _width�� �׸����� ���� �� ���� ũ��
    for (int i = 0; i <= _length; ++i)
    {
        for (int j = 0; j <= _width; ++j)
        {
            Vertex vertex;

            vertex.position.x = (j - (_width / 2)) * size * _scale.x; // x�� ��ġ
            vertex.position.y = 0; // y�� ����
            vertex.position.z = (i - (_length / 2)) * -size * _scale.z; // z�� ��ġ (������ �����Ͽ� ���� ���߱�)

            int heightIndex = i * _width + j; // ����Ʈ�ʿ��� �ε��� ���

            if (heightIndex < _width * _length) {
                vertex.position.y = static_cast<float>(_HeightMapPixels[heightIndex]) * _scale.y; // ������ ����
            }

            vertex.uv.x = static_cast<float>(j) / _width;  
            vertex.uv.y = static_cast<float>(i) / _length; 

            v.push_back(vertex);
        }
    }

    vector<uint32_t> index;

    for (int i = 0; i < _length-1; ++i)
    {
        for (int j = 0; j < _width-1; ++j)
        {
            // ù ��° �ﰢ��
            index.push_back(i * (_width + 1) + j); // ���� ��
            index.push_back(i * (_width + 1) + (j + 1)); // ������ ��
            index.push_back((i + 1) * (_width + 1) + j); // �Ʒ��� ��

            // �� ��° �ﰢ��
            index.push_back(i * (_width + 1) + (j + 1)); // ������ ��
            index.push_back((i + 1) * (_width + 1) + (j + 1)); // �Ʒ��� ������ ��
            index.push_back((i + 1) * (_width + 1) + j); // �Ʒ��� ��


        }
    }

    _mesh->Init(v, index);
}

void Terrain::LoadFIle(LPCTSTR pFileName, vec3 scale)
{

    _scale = scale;

    // ���� ����
    HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

    // ���� ���� ���� �� ũ���� �߻�
    assert(hFile != INVALID_HANDLE_VALUE && "Failed to open height map file!");

    // ���� ũ�� ��������
    DWORD fileSize = ::GetFileSize(hFile, NULL);
    assert(fileSize != INVALID_FILE_SIZE && "Failed to get height map file size!");

    // �ȼ��� ����Ʈ �� (8��Ʈ = 1����Ʈ, 16��Ʈ = 2����Ʈ)
    int bytesPerPixel = 1; // RAW ������ 8��Ʈ��� ����

    // �ػ� ���
    _width = _length = static_cast<int>(sqrt(fileSize / bytesPerPixel));

    // ���� ũ��� ���� ũ�Ⱑ �´��� Ȯ��
    assert(_width * _length * bytesPerPixel == fileSize && "File size doesn't match calculated dimensions!");

    // ����Ʈ�� �����͸� ���� �޸� �Ҵ�
    _HeightMapPixels = new BYTE[_width * _length];

    // ���Ͽ��� ������ �б�
    DWORD dwBytesRead;
    BOOL bReadSuccess = ::ReadFile(hFile, _HeightMapPixels, fileSize, &dwBytesRead, NULL);

    // ���� �б� ���� �� ũ���� �߻�
    assert(bReadSuccess && "Failed to read height map file!");
    assert(dwBytesRead == fileSize && "Incomplete file read!");

    // ���� �ڵ� �ݱ�
    ::CloseHandle(hFile);

}


