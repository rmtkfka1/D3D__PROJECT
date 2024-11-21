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

Terrain::Terrain():CustomObject(PlayerType::Terrain)
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
    LoadFIle(L"../Resources/Texture/heightMap/heightMap.raw", vec3(30.0f, 20.0f, 30.0f));
    CreateMesh();

    ShaderInfo info;
  
    shared_ptr<GraphicsShader> shader = make_shared<GraphicsShader>();
    shader->Init(L"terrain.hlsl", info);
    this->SetShader(shader);

    {
        shared_ptr<Texture> texture = make_shared<Texture>();
        texture->Init(L"heightMap/Base_Texture.dds");
        _material->SetDiffuseTexture(texture);
    }

    {
        shared_ptr<Texture> texture = make_shared<Texture>();
        texture->Init(L"heightMap/diffuse.tga");
        _material->SetNormalTexture(texture);
    }

    {
        shared_ptr<Texture> texture = make_shared<Texture>();
        texture->Init(L"heightMap/Lava(Emissive).dds");
        _material->SetSpecularTexture(texture);
    }

}

void Terrain::Update()
{
    _transform->Update();
}

void Terrain::Render()
{
    _shader->SetPipelineState();
    _transform->PushData();
    _material->PushData();
    core->GetBufferManager()->GetGraphicsTableHeap()->SetGraphicsRootDescriptorTable();
    _mesh->Render();
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER
vec3 Terrain::GetHeight(vec3 pos)
{
    float normalizedX = pos.x + (_width * _scale.x) / 2;
    float normalizedZ = -pos.z + (_length * _scale.z) / 2;

    // 하이트맵의 인덱스 계산
    int xIndex = static_cast<int>(normalizedX / _scale.x);
    int zIndex = static_cast<int>(normalizedZ / _scale.z);

    // 인덱스 유효성 검사
    if (xIndex < 0 || xIndex >= _width - 1 || zIndex < 0 || zIndex >= _length - 1)
    {
        return vec3(pos.x, 0.0f, pos.z); // 범위를 벗어난 경우 기본 높이 반환
    }

    // 하이트맵에서 비율 계산
    float fxPercent = (normalizedX / _scale.x) - xIndex;
    float fzPercent = (normalizedZ / _scale.z) - zIndex;

    // 하이트맵에서 높이값 가져오기
    int heightIndex = zIndex * _width + xIndex;

    float bottomLeft = static_cast<float>(_HeightMapPixels[heightIndex]);
    float bottomRight = static_cast<float>(_HeightMapPixels[heightIndex + 1]);
    float topLeft = static_cast<float>(_HeightMapPixels[heightIndex + _width]);
    float topRight = static_cast<float>(_HeightMapPixels[heightIndex + _width + 1]);

    // 선형 보간
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
    // 사각형의 분할 방식에 따라 보간을 조정
    if (fzPercent < (1.0f - fxPercent))
    {
        // 오른쪽 삼각형의 높이값 조정
        topRight = topLeft + (bottomRight - bottomLeft);
    }
    else
    {
        // 왼쪽 삼각형의 높이값 조정
        bottomLeft = topLeft + (bottomRight - topRight);
    }
#endif

    // 실제 높이값 계산
    float topHeight = topLeft * (1 - fxPercent) + topRight * fxPercent;
    float bottomHeight = bottomLeft * (1 - fxPercent) + bottomRight * fxPercent;
    float height = bottomHeight * (1 - fzPercent) + topHeight * fzPercent;

    // 높이값을 포함한 vec3 반환 (x, height, z)
    return vec3(pos.x, height * _scale.y, pos.z);
};

void Terrain::CreateMesh()
{

  
    int size = 1;
    vector<Vertex> v;

    for (int i = 0; i <= _length; ++i)
    {
        for (int j = 0; j <= _width; ++j)
        {
            Vertex vertex;

            vertex.position.x = (j - (_width / 2)) * size * _scale.x;
            vertex.position.y = 0; // 초기화
            vertex.position.z = (i - (_length / 2)) * -size * _scale.z;

            int heightIndex = i * _width + j;

            if (heightIndex < _width * _length) {
                vec3 Height = GetHeight(vec3(vertex.position.x, 0, vertex.position.z));
                vertex.position.y = Height.y ;
            }

            vertex.uv.x = static_cast<float>(j) / _width;
            vertex.uv.y = static_cast<float>(i) / _length;

            v.push_back(vertex);
        }
    }

    for (int i = 0; i <= _length; ++i)
    {
        for (int j = 0; j <= _width; ++j)
        {
            Vertex& vertex = v[i * (_width + 1) + j]; // 현재 정점

            if (i > 0 && i < _length - 1 && j > 0 && j < _width - 1)
            {
                vec3 left = v[i * (_width + 1) + (j - 1)].position;
                vec3 right = v[i * (_width + 1) + (j + 1)].position;
                vec3 top = v[(i - 1) * (_width + 1) + j].position;
                vec3 bottom = v[(i + 1) * (_width + 1) + j].position;

                vec3 v1 = right - left;
                vec3 v2 = bottom - top;

                vec3 normal = v1.Cross(v2);
                normal.Normalize();
                vertex.normal = normal;
            }
            else
            {
                vertex.normal = vec3(0, 1, 0); // 기본 노말 설정
            }
        }
    }

    vector<uint32_t> index;

    for (int i = 0; i < _length - 1; ++i)
    {
        for (int j = 0; j < _width - 1; ++j)
        {
            index.push_back(i * (_width + 1) + j);
            index.push_back(i * (_width + 1) + (j + 1));
            index.push_back((i + 1) * (_width + 1) + j);

            index.push_back(i * (_width + 1) + (j + 1));
            index.push_back((i + 1) * (_width + 1) + (j + 1));
            index.push_back((i + 1) * (_width + 1) + j);
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
    if (_HeightMapPixels==nullptr)
    {
        _HeightMapPixels = new BYTE[_width * _length];
    }
    // 파일에서 데이터 읽기
    DWORD dwBytesRead;
    BOOL bReadSuccess = ::ReadFile(hFile, _HeightMapPixels, fileSize, &dwBytesRead, NULL);

    // 파일 읽기 실패 시 크래시 발생
    assert(bReadSuccess && "Failed to read height map file!");
    assert(dwBytesRead == fileSize && "Incomplete file read!");

    // 파일 핸들 닫기
    ::CloseHandle(hFile);

}


