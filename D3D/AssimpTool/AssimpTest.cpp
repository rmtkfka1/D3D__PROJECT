#include "pch.h"
#include "AssimpTest.h"
#include "Utils.h"

AssimpTest::AssimpTest()
{
	_importer = make_shared<Assimp::Importer>();
}

AssimpTest::~AssimpTest()
{
}

void AssimpTest::Init(const wstring& path)
{
	wstring fileStr = _assetPath + path;
	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

	std::cout << "Extension: " << p.extension().string() << std::endl;

	_scene = _importer->ReadFile(
		Utils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);

	//ProcessMesh(_scene->mRootNode);
	
	cout << "===================" << endl;

	ProcessAni();


}

void AssimpTest::ProcessMesh(aiNode* node)
{
	cout << "노드의 이름:" << node->mName.C_Str() << endl;

	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh=_scene->mMeshes[i];

		for (uint32 v = 0; v < mesh->mNumVertices; v++)
		{
			// Vertex
			Vertex vertex;
			::memcpy(&vertex.position, &mesh->mVertices[v], sizeof(vec3));
		}

	}

	uint32 numChild = node->mNumChildren;

	for (int i = 0; i < numChild; ++i)
	{ 
		ProcessMesh(node->mChildren[i]);
	}
}

void AssimpTest::ProcessAni()
{
	cout << "애니메이션 갯수:"<< _scene->mNumAnimations << endl;

	if (_scene->mNumAnimations <= 0)
		return;

	for (unsigned int i = 0; i < _scene->mNumAnimations; ++i) 
	{
		aiAnimation* animation = _scene->mAnimations[i];
		std::cout << "Animation Name: " << animation->mName.C_Str() << std::endl;
		std::cout << "Duration: " << animation->mDuration << " ticks" << std::endl;
		std::cout << "Ticks Per Second: " << animation->mTicksPerSecond << std::endl;

		for (unsigned int j = 0; j < animation->mNumChannels; ++j) 
		{
			aiNodeAnim* channel = animation->mChannels[j];
			std::cout << "  Channel for Node: " << channel->mNodeName.C_Str() << std::endl;

			// 위치 키프레임
			std::cout << "    Position Keys: " << channel->mNumPositionKeys << std::endl;

			for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k) 
			{
				aiVectorKey key = channel->mPositionKeys[k];
				std::cout << "      Time: " << key.mTime
					<< ", Position: (" << key.mValue.x << ", "
					<< key.mValue.y << ", " << key.mValue.z << ")" << std::endl;
			}

			// 회전 키프레임
			std::cout << "    Rotation Keys: " << channel->mNumRotationKeys << std::endl;
			/*for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k) 
			{
				aiQuatKey key = channel->mRotationKeys[k];
				std::cout << "      Time: " << key.mTime
					<< ", Rotation: (" << key.mValue.w << ", "
					<< key.mValue.x << ", " << key.mValue.y << ", "
					<< key.mValue.z << ")" << std::endl;
			}*/

			// 스케일 키프레임
			std::cout << "    Scaling Keys: " << channel->mNumScalingKeys << std::endl;
			/*for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k) 
			{
				aiVectorKey key = channel->mScalingKeys[k];
				std::cout << "      Time: " << key.mTime
					<< ", Scale: (" << key.mValue.x << ", "
					<< key.mValue.y << ", " << key.mValue.z << ")" << std::endl;
			}*/
		}
	}


}
