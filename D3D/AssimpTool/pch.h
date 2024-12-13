#pragma once

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
#include <iostream>
#include <set>

#include <stack>
#include <queue>

using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

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
#include "SimpleMath.h"
#include "SimpleMath.inl"
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

#include "FileUtils.h"

struct Vertex
{
	vec3 position = { 0, 0, 0 };
	vec2 uv = { 0, 0 };
	vec3 normal = { 0, 0, 0 };
	vec3 tangent = { 0, 0, 0 };
	vec4 blendIndices = { 0,0,0,0 };
	vec4 blendWeights = { 0,0,0,0 };
};

struct asBone
{
	string name;
	int32 index = -1;
	int32 parent = -1;
	Matrix transform;
};

struct asMesh
{
	string name{};
	vector<Vertex> vertices;
	vector<uint32> indices;

	int32 boneIndex{};
	string materialName{};

	BoundingBox box;
	BoundingSphere sphere;



};

struct asMaterial
{
	string name{};
	Color ambient{};
	Color diffuse{};
	Color specular{};
	Color emissive{};
	string diffuseFile{};
	string specularFile{};
	string normalFile{};
};

enum  class DataType
{
	STATIC,
	HIEARCY,
	ANIMATION
};



//스키닝데이터추출.
struct asBlendWeight
{
	void Set(uint32 index, uint32 boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: indices.x = i; weights.x = w; break;
		case 1: indices.y = i; weights.y = w; break;
		case 2: indices.z = i; weights.z = w; break;
		case 3: indices.w = i; weights.w = w; break;
		}
	}

	vec4 indices = vec4(0, 0, 0, 0);
	vec4 weights = vec4(0, 0, 0, 0);
};

// 정점마다 -> (관절번호, 가중치)
struct asBoneWeights
{
	void AddWeights(uint32 boneIndex, float weight)
	{
		if (weight <= 0.0f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[weight](const Pair& p) { return weight > p.second; });

		boneWeights.insert(findIt, Pair(boneIndex, weight));
	}

	asBlendWeight GetBlendWeights()
	{
		asBlendWeight blendWeights;

		for (uint32 i = 0; i < boneWeights.size(); i++)
		{
			if (i >= 4)
				break;

			blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
		}

		return blendWeights;
	}

	void Normalize()
	{
		if (boneWeights.size() >= 4)
			boneWeights.resize(4);

		float totalWeight = 0.f;
		for (const auto& item : boneWeights)
			totalWeight += item.second;

		float scale = 1.f / totalWeight;
		for (auto& item : boneWeights)
			item.second *= scale;
	}

	using Pair = pair<int32, float>;
	vector<Pair> boneWeights;
};


//애니메이션 클립 데이터
struct asKeyframeData
{
	float time;
	vec3 scale;
	Quaternion rotation;
	vec3 translation;
};

struct asKeyframe
{
	string boneName;
	vector<asKeyframeData> transforms;
};

struct asAnimation
{
	string name;
	uint32 frameCount; //몇프레임짜리인지
	float frameRate; // 몇프레임으로 돌아갈지
	float duration; // 지속시간
	vector<shared_ptr<asKeyframe>> keyframes;
};

// Cache
struct asKeyFrameTemp
{
	aiString name;
	vector<asKeyframeData> keyframe;
};