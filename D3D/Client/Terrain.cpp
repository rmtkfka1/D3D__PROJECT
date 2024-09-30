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
    LoadFIle(L"../Resources/Texture/heightMap/heightMap.raw", vec3(1, 1, 1));
    CreateMesh();

    ShaderInfo info;
    /*   info.rasterizerType = RASTERIZER_TYPE::WIREFRAME;*/

    shared_ptr<Shader> shader = make_shared<Shader>();
    shader->Init(L"default.hlsl", info);

    shared_ptr<Texture> texture = make_shared<Texture>();
    texture->Init(L"heightMap/Base_Texture.dds");

    _material->SetShader(shader);
    _material->SetDiffuseTexture(texture);
}

void Terrain::Update()
{
    _transform->SetLocalScale(vec3(5.0f, 1.0f, 5.0f));
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
    std::vector<Vertex> vertices;
    float startX = 0.0f;
    float startZ = 0.0f;
    float gridSize = 1.0f; // �׸��� ������ ������ �� �ִ� ����

    for (int z = 0; z <= _length; ++z)
    {
        for (int x = 0; x <= _width; ++x)
        {
            Vertex temp;

            // ����Ʈ�� �����͸� ����Ͽ� y ��(����) ����
            int heightIndex = z * _width + x;
            float height = _HeightMapPixels[heightIndex]; // ������ ����� ����

            temp.position.x = (startX + x * gridSize); // x �� ������ ����
            temp.position.y = height;
            temp.position.z = (startZ + z * gridSize); // z �� ������ ����

            // UV ��ǥ ����
            temp.uv.x = static_cast<float>(x) / _width;  // u = x ��ǥ�� ����ȭ
            temp.uv.y = static_cast<float>(z) / _length; // v = z ��ǥ�� ����ȭ

            // �ӽ� ��� ��, ���� ��� ����� ���߿�
            temp.normal = vec3(0, 1, 0);

            vertices.push_back(temp);
        }
    }

    // �ε��� �迭 ���� (�ﰢ���� �׸��� ����)
    std::vector<uint32> indices;
    for (int z = 0; z < _length-1; ++z)
    {
        for (int x = 0; x < _width-1; ++x)
        {
            int topLeft = z * (_width + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (_width + 1);
            int bottomRight = bottomLeft + 1;

            // ù ��° �ﰢ��
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            // �� ��° �ﰢ��
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }

    _mesh->Init(vertices, indices); // �޽��� ������ �ε��� ����

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


