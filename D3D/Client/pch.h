#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _HAS_STD_BYTE 0

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <stack>
#include <queue>

using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
#include "simple_mesh_LH.h"
#include "SimpleMath_LH.inl"
#include "tinyxml2.h"
// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")



#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")
#else
#pragma comment(lib, "Assimp/assimp-vc143-mt.lib")
#endif

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Color = DirectX::XMFLOAT4;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;


extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

const uint32 SWAP_CHAIN_FRAME_COUNT = 4;
const uint32 MAX_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;

#define LOAD(type, path, ...) ResourceManager::GetInstance()->Load<type>(path, __VA_ARGS__)

inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        throw std::exception();
    }
}

struct Vertex
{
    vec3 position = { 0, 0, 0 };
    vec2 uv = { 0, 0 };
    vec3 normal = { 0, 0, 0 };
    vec3 tangent = { 0, 0, 0 };
};


struct TransformParams
{
    Matrix matWorld;
};

struct CameraParams
{
    Matrix matView;
    Matrix matProjection;
};


extern unique_ptr<class Core> core;

#include "KeyManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "CollisonManager.h"
#include "BufferManager.h"
#include "Compute.h"
#include "Graphics.h"
#define GRAPHICS core->GetGraphics()
#define COMPUTE core->GetCompute()

