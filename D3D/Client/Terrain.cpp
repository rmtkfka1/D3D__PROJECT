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
    float gridSize = 1.0f; // 그리드 간격을 조정할 수 있는 변수

    for (int z = 0; z <= _length; ++z)
    {
        for (int x = 0; x <= _width; ++x)
        {
            Vertex temp;

            // 하이트맵 데이터를 사용하여 y 값(높이) 설정
            int heightIndex = z * _width + x;
            float height = _HeightMapPixels[heightIndex]; // 스케일 적용된 높이

            temp.position.x = (startX + x * gridSize); // x 축 스케일 적용
            temp.position.y = height;
            temp.position.z = (startZ + z * gridSize); // z 축 스케일 적용

            // UV 좌표 설정
            temp.uv.x = static_cast<float>(x) / _width;  // u = x 좌표를 정규화
            temp.uv.y = static_cast<float>(z) / _length; // v = z 좌표를 정규화

            // 임시 노멀 값, 실제 노멀 계산은 나중에
            temp.normal = vec3(0, 1, 0);

            vertices.push_back(temp);
        }
    }

    // 인덱스 배열 설정 (삼각형을 그리기 위해)
    std::vector<uint32> indices;
    for (int z = 0; z < _length-1; ++z)
    {
        for (int x = 0; x < _width-1; ++x)
        {
            int topLeft = z * (_width + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + (_width + 1);
            int bottomRight = bottomLeft + 1;

            // 첫 번째 삼각형
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            // 두 번째 삼각형
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }

    _mesh->Init(vertices, indices); // 메쉬에 정점과 인덱스 전달

}

void Terrain::LoadFIle(LPCTSTR pFileName, vec3 scale)
{

    _scale = scale;

    // 파일 열기
    HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);

    // 파일 열기 실패 시 크래시 발생
    assert(hFile != INVALID_HANDLE_VALUE && "Failed to open height map file!");

    // 파일 크기 가져오기
    DWORD fileSize = ::GetFileSize(hFile, NULL);
    assert(fileSize != INVALID_FILE_SIZE && "Failed to get height map file size!");

    // 픽셀당 바이트 수 (8비트 = 1바이트, 16비트 = 2바이트)
    int bytesPerPixel = 1; // RAW 파일이 8비트라고 가정

    // 해상도 계산
    _width = _length = static_cast<int>(sqrt(fileSize / bytesPerPixel));

    // 파일 크기와 계산된 크기가 맞는지 확인
    assert(_width * _length * bytesPerPixel == fileSize && "File size doesn't match calculated dimensions!");

    // 하이트맵 데이터를 담을 메모리 할당
    _HeightMapPixels = new BYTE[_width * _length];

    // 파일에서 데이터 읽기
    DWORD dwBytesRead;
    BOOL bReadSuccess = ::ReadFile(hFile, _HeightMapPixels, fileSize, &dwBytesRead, NULL);

    // 파일 읽기 실패 시 크래시 발생
    assert(bReadSuccess && "Failed to read height map file!");
    assert(dwBytesRead == fileSize && "Incomplete file read!");

    // 파일 핸들 닫기
    ::CloseHandle(hFile);

}


