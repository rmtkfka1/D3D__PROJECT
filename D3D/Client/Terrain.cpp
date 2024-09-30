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

    // _length와 _width는 그리드의 세로 및 가로 크기
    for (int i = 0; i <= _length; ++i)
    {
        for (int j = 0; j <= _width; ++j)
        {
            Vertex vertex;

            vertex.position.x = (j - (_width / 2)) * size * _scale.x; // x축 위치
            vertex.position.y = 0; // y축 높이
            vertex.position.z = (i - (_length / 2)) * -size * _scale.z; // z축 위치 (음수로 설정하여 방향 맞추기)

            int heightIndex = i * _width + j; // 하이트맵에서 인덱스 계산

            if (heightIndex < _width * _length) {
                vertex.position.y = static_cast<float>(_HeightMapPixels[heightIndex]) * _scale.y; // 스케일 적용
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
            // 첫 번째 삼각형
            index.push_back(i * (_width + 1) + j); // 왼쪽 점
            index.push_back(i * (_width + 1) + (j + 1)); // 오른쪽 점
            index.push_back((i + 1) * (_width + 1) + j); // 아래쪽 점

            // 두 번째 삼각형
            index.push_back(i * (_width + 1) + (j + 1)); // 오른쪽 점
            index.push_back((i + 1) * (_width + 1) + (j + 1)); // 아래쪽 오른쪽 점
            index.push_back((i + 1) * (_width + 1) + j); // 아래쪽 점


        }
    }

    _mesh->Init(v, index);
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


