#include "pch.h"
#include "Converter.h"
#include <filesystem>
#include "tinyxml2.h"
#include "fstream"
#include "Utils.h"

#define NOMINMAX
#define _HAS_STD_BYTE 0

Converter::Converter()
{
	_importer = make_shared<Assimp::Importer>();
}

Converter::~Converter()
{

}

void Converter::ReadAssetFile(wstring file)
{
	wstring fileStr = _assetPath + file;
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
}

void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}

void Converter::ExportAnimationData(wstring savePath, uint32 index)
{
	wstring finalPath = _modelPath + savePath + L".clip";
	assert(index < _scene->mNumAnimations);
	shared_ptr<asAnimation> animation = ReadAnimationData(_scene->mAnimations[index]);
	PrintAnimationData(animation);
	WriteAnimationData(animation, finalPath);
}

void Converter::ExportModelData(wstring savePath, DataType type)
{
	_type = type;

	wstring finalPath = _modelPath + savePath + L".mesh";
	Matrix tr = Matrix::Identity;

	cout << "메쉬 갯수:" << _scene->mNumMeshes << endl;

	for (unsigned int i = 0; i < _scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = _scene->mMeshes[i];
		if (mesh->mName.length > 0)
		{
			cout << "메쉬 이름: " << mesh->mName.C_Str() << endl;
		}
		else
		{
			cout << "메쉬 이름: (없음)" << endl;
		}
	}

	ReadModelData(_scene->mRootNode, -1, -1, tr);
	ReadSkinData();
	WriteModelFile(finalPath);
}



void Converter::PrintAnimationData(shared_ptr<asAnimation> animation)
{
	cout << animation->name << endl;
	cout << animation->duration << endl;
	cout << animation->frameCount << endl;
	cout << animation->frameRate << endl;

	cout << endl;


	for (auto& ele : animation->keyframes)
	{
		cout << ele->boneName << endl;
		int size = ele->transforms.size();
		cout << size << endl;


	}
}

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent, DirectX::SimpleMath::Matrix tr)
{
	shared_ptr<asBone> bone = make_shared<asBone>();
	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();

	// Relative Transform
	Matrix transform(node->mTransformation[0]);
	bone->transform = transform.Transpose();

	// 2) Root (Local)
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = _bones[parent]->transform;

	////// Local (Root) Transform
	if (_type == DataType::STATIC || _type == DataType::ANIMATION)
	{
		bone->transform = bone->transform * matParent;
	}

	else
	{
		bone->transform = bone->transform;
	}

	_bones.push_back(bone);

	ReadMeshData(node, index, bone->transform);

	for (uint32 i = 0; i < node->mNumChildren; i++)
	{
		ReadModelData(node->mChildren[i], _bones.size(), index, bone->transform);
	}
}

void Converter::ReadMeshData(aiNode* node, int32 bone, DirectX::SimpleMath::Matrix m)
{
	if (node->mNumMeshes < 1)
		return;

	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		uint32 index = node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// Material Name
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		const uint32 startVertex = mesh->vertices.size();

		for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			Vertex vertex;

			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(vec3));

			// UV
			if (srcMesh->HasTextureCoords(0))
				::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(vec2));

			// Normal
			if (srcMesh->HasNormals())
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(vec3));

			if (srcMesh->HasTangentsAndBitangents())
				::memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(vec3));

			mesh->vertices.push_back(vertex);

		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; k++)
				mesh->indices.push_back(face.mIndices[k] + startVertex);
		}
	}

	if (_type == DataType::STATIC)
	{
		for (auto& v : mesh->vertices)
		{
			v.position = DirectX::SimpleMath::Vector3::Transform(v.position, m);
			v.normal = DirectX::SimpleMath::Vector3::TransformNormal(v.normal, m);
		}
	}

	_meshes.push_back(mesh);


}

void Converter::WriteModelFile(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directory(path.parent_path());

	string folder = path.parent_path().string();

	ofstream out{ path ,ios::binary };

	////////////////////////////////////////////////////////////////////
	////bone 정보 추출하여 기록/////////////////////////////////////////
	{
		int count = _bones.size();
		out.write((char*)&count, sizeof(int));

		for (auto bone : _bones)
		{
			size_t len = bone->name.size();
			out.write((const char*)&len, sizeof(size_t));
			out.write(bone->name.data(), len);
			out.write((const char*)&bone->index, sizeof(int32));
			out.write((const char*)&bone->parent, sizeof(int32));

			{
				//매트릭스 정보 기록 
				XMFLOAT4X4 data;
				XMStoreFloat4x4(&data, bone->transform);
				out.write((char*)(&data), sizeof(XMFLOAT4X4));
			}
		}
	}


	//////////////////////////////////////////////////////////////////
	{

		int count = _meshes.size();
		out.write((char*)&count, sizeof(int));

		for (auto& mesh : _meshes)
		{


			{
				//메시 이름기록
				size_t len = mesh->name.size();
				out.write((const char*)&len, sizeof(size_t));
				out.write((const char*)mesh->name.data(), len);
			}


			{
				//boneindex
				out.write((const char*)&mesh->boneIndex, sizeof(int32));
			}

			{

				//materialnmae
				size_t len = mesh->materialName.size();
				out.write((const char*)&len, sizeof(size_t));
				out.write((const char*)mesh->materialName.data(), len);
			}


			{
				//vertices 기록
				size_t len = mesh->vertices.size();
				out.write((const char*)&len, sizeof(size_t));


				for (int i = 0; i < len; ++i)
				{
					out.write((const char*)&mesh->vertices[i].position, sizeof(vec3));
					out.write((const char*)&mesh->vertices[i].uv, sizeof(vec2));
					out.write((const char*)&mesh->vertices[i].normal, sizeof(vec3));
					out.write((const char*)&mesh->vertices[i].tangent, sizeof(vec3));
					out.write((const char*)&mesh->vertices[i].blendIndices, sizeof(vec4));
					out.write((const char*)&mesh->vertices[i].blendWeights, sizeof(vec4));
				}
			}


			{

				//indices 기록
				uint32 len = mesh->indices.size();
				out.write((const char*)&len, sizeof(uint32));

				for (int i = 0; i < len; ++i)
				{
					out.write((const char*)(&mesh->indices[i]), sizeof(uint32));
				};
			}

			{

				//메쉬 별로 중점 , 사이즈 정보기록
				out.write((const char*)&mesh->box.Center, sizeof(vec3));
				out.write((const char*)&mesh->box.Extents, sizeof(vec3));

			}




		}



	}

	CalculateBoundingBox();

	{
		//total bounding box 의 사이즈와 센터정보 기록.

		out.write((const char*)&_totalbox.Center, sizeof(vec3));
		out.write((const char*)&_totalbox.Extents, sizeof(vec3));
		out.write((const char*)&_totalSphere.Radius, sizeof(float));
	}



}
void Converter::CalculateBoundingBox()
{
	if (_type == DataType::HIEARCY)
	{
		for (int i = 0; i < _meshes.size(); ++i)
		{
			Matrix m = Matrix::Identity;

			int boneIndex = _meshes[i]->boneIndex;

			while (boneIndex >= 0) // 부모 본까지 트랜스폼을 누적 곱셈
			{
				m *= _bones[boneIndex]->transform;
				boneIndex = _bones[boneIndex]->parent; // 부모 본 인덱스를 참조
			}

			for (int j = 0; j < _meshes[i]->vertices.size(); ++j)
			{
				_meshes[i]->vertices[j].position = DirectX::SimpleMath::Vector3::Transform(
					_meshes[i]->vertices[j].position, m);
			}
		}
	}

	for (int i = 0; i < _meshes.size(); ++i)
	{
		auto& box = _meshes[i]->box;
		auto& sphere = _meshes[i]->sphere;

		BoundingBox::CreateFromPoints(box, (size_t)_meshes[i]->vertices.size(), &_meshes[i]->vertices[0].position, (size_t)sizeof(Vertex));
		BoundingSphere::CreateFromPoints(sphere, (size_t)_meshes[i]->vertices.size(), &_meshes[i]->vertices[0].position, (size_t)sizeof(Vertex));

		cout << "new :" << _meshes[i]->name << endl;
		cout << "사이즈" << box.Extents.x << " , " << box.Extents.y << " ," << box.Extents.z << endl;
		cout << "중심점" << box.Center.x << " , " << box.Center.y << " ," << box.Center.z << endl;
		cout << "===========================================================" << endl;

		sphere.Center = box.Center;

		cout << "new :" << _meshes[i]->name << endl;
		cout << "반지름" << sphere.Radius << endl;
		cout << "중심점" << sphere.Center.x << " , " << sphere.Center.y << " ," << sphere.Center.z << endl;
		cout << "===========================================================" << endl;

	}

	_totalbox = _meshes[0]->box;
	_totalSphere = _meshes[0]->sphere;

	for (int i = 1; i < _meshes.size(); ++i)
	{
		BoundingBox::CreateMerged(_totalbox, _totalbox, _meshes[i]->box);
		BoundingSphere::CreateMerged(_totalSphere, _totalSphere, _meshes[i]->sphere);
	}

	cout << "totalBox" << endl;
	cout << "사이즈" << _totalbox.Extents.x << " , " << _totalbox.Extents.y << " ," << _totalbox.Extents.z << endl;
	cout << "중심점" << _totalbox.Center.x << " , " << _totalbox.Center.y << " ," << _totalbox.Center.z << endl;
	cout << "===========================================================" << endl;


	cout << "totalSphere" << endl;
	cout << "반지름" << _totalSphere.Radius << endl;
	cout << "중심점" << _totalSphere.Center.x << " , " << _totalSphere.Center.y << " ," << _totalSphere.Center.z << endl;
	cout << "===========================================================" << endl;

};
void Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i];

		shared_ptr<asMaterial> material = make_shared<asMaterial>();
		material->name = srcMaterial->GetName().C_Str();

		aiColor3D color;
		// Ambient
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Color(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = Color(color.r, color.g, color.b, 1.0f);

		aiString file;

		// Diffuse Texture
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->diffuseFile = file.C_Str();

		// Normal Texture
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->normalFile = file.C_Str();

		// Specular Texture
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->specularFile = file.C_Str();

		_materials.push_back(material);
	}
}

void Converter::WriteMaterialData(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directory(path.parent_path());

	string folder = path.parent_path().string();

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);


	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(Utils::ToString(finalPath).c_str());
}

void Converter::ReadSkinData()
{

	for (int32 i = 0; i < _scene->mNumMeshes; ++i)
	{
		aiMesh* srcMesh = _scene->mMeshes[i];

		if (srcMesh->HasBones() == false)
			continue;

		if (i >= _meshes.size())
			continue;

		shared_ptr<asMesh> mesh = _meshes[i];
		vector<asBoneWeights> temp;
		temp.resize(mesh->vertices.size());

		for (int32 b = 0; b < srcMesh->mNumBones; ++b)
		{
			aiBone* srcMeshBone = srcMesh->mBones[b];

			uint32 boneIndex = GetBoneIndex(srcMeshBone->mName.C_Str());

			for (uint32 w = 0; w < srcMeshBone->mNumWeights; ++w)
			{
				uint32 vertexId = srcMeshBone->mWeights[w].mVertexId;
				float weight = srcMeshBone->mWeights[w].mWeight;

				temp[vertexId].AddWeights(boneIndex, weight);
			}
		}

		for (uint32 v = 0; v < temp.size(); ++v)
		{
			temp[v].Normalize();

			asBlendWeight blendWeight = temp[v].GetBlendWeights();
			mesh->vertices[v].blendIndices = blendWeight.indices;
			mesh->vertices[v].blendWeights = blendWeight.weights;
		}
	}

}

void Converter::WriteAnimationData(shared_ptr<asAnimation> animation, wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	filesystem::create_directory(path.parent_path());

	ofstream out{ path ,ios::binary };

	//애니메이션의 이름기록
	size_t len = animation->name.length();
	out.write((const char*)&len, sizeof(len));
	out.write((const char*)animation->name.data(), len);


	out.write((const char*)&animation->frameRate, sizeof(float));
	out.write((const char*)&animation->frameCount, sizeof(uint32));

	size_t size = animation->keyframes.size();
	out.write((const char*)&size, sizeof(size));


	for (auto& ele : animation->keyframes)
	{
		size_t len = ele->boneName.length();
		out.write((const char*)&len, sizeof(len));

		out.write((const char*)ele->boneName.data(), len);

		size_t size = ele->transforms.size();
		out.write((const char*)&size, sizeof(size));

		for (int i = 0; i < size; ++i)
		{
			out.write((const char*)&ele->transforms[i].time, sizeof(float));
			out.write((const char*)&ele->transforms[i].scale, sizeof(vec3));
			out.write((const char*)&ele->transforms[i].rotation, sizeof(Quaternion));
			out.write((const char*)&ele->transforms[i].translation, sizeof(vec3));
		}

	}

}

uint32 Converter::GetBoneIndex(const string& name)
{
	for (auto& bone : _bones)
	{
		if (bone->name == name)
		{
			return bone->index;
		}
	}

	assert(false);

}

std::string Converter::WriteTexture(string saveFolder, string file)
{
	string fileName = filesystem::path(file).filename().string();
	string folderName = filesystem::path(saveFolder).filename().string();


	string originStr = (filesystem::path(_assetPath) / folderName / file).string();
	Utils::Replace(originStr, "\\", "/");

	string pathStr = (filesystem::path(saveFolder) / fileName).string();
	Utils::Replace(pathStr, "\\", "/");

	::CopyFileA(originStr.c_str(), pathStr.c_str(), false);

	return fileName;
}
shared_ptr<asAnimation> Converter::ReadAnimationData(aiAnimation* source)
{
	shared_ptr<asAnimation> animation = make_shared<asAnimation>();
	animation->name = source->mName.C_Str();
	animation->frameRate = source->mTicksPerSecond;
	animation->frameCount = source->mDuration + 1;

	map<string, shared_ptr<asKeyFrameTemp>> cacheAnimNodes;

	for (uint32 i = 0; i < source->mNumChannels; ++i)
	{
		aiNodeAnim* srcNode = source->mChannels[i];

		shared_ptr<asKeyFrameTemp> node = ParseAnimationNode(animation, srcNode);

		// 현재 찾은 노드 중에 제일 긴 시간으로 애니메이션 시간 갱신
		animation->duration = max(animation->duration, node->keyframe.back().time);
		cacheAnimNodes[srcNode->mNodeName.C_Str()] = node;

	}

	ReadKeyframeData(animation, _scene->mRootNode, cacheAnimNodes);

	return animation;
}
shared_ptr<asKeyFrameTemp> Converter::ParseAnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode)
{
	shared_ptr<asKeyFrameTemp> node = make_shared<asKeyFrameTemp>();

	node->name = srcNode->mNodeName;

	uint32 keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumRotationKeys), srcNode->mNumScalingKeys);

	for (uint32 k = 0; k < keyCount; k++)
	{
		asKeyframeData frameData;

		bool found = false;

		uint32 t = node->keyframe.size();

		// Position

		if (k < srcNode->mNumPositionKeys && fabsf((float)srcNode->mPositionKeys[k].mTime - (float)t) <= 0.0001f) //키없는거 방지용max,max 쓰기때문에
		{
			aiVectorKey key = srcNode->mPositionKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.translation, sizeof(vec3), &key.mValue, sizeof(aiVector3D));
			found = true;
		}

		// Rotation
		if (k < srcNode->mNumRotationKeys && fabsf((float)srcNode->mRotationKeys[k].mTime - (float)t) <= 0.0001f)//키없는거 방지용
		{
			aiQuatKey key = srcNode->mRotationKeys[k];
			frameData.time = (float)key.mTime;

			frameData.rotation.x = key.mValue.x;
			frameData.rotation.y = key.mValue.y;
			frameData.rotation.z = key.mValue.z;
			frameData.rotation.w = key.mValue.w;

			found = true;
		}

		// Scale
		if (k < srcNode->mNumScalingKeys && fabsf((float)srcNode->mScalingKeys[k].mTime - (float)t) <= 0.0001f)//키없는거 방지용
		{
			aiVectorKey key = srcNode->mScalingKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.scale, sizeof(vec3), &key.mValue, sizeof(aiVector3D));

			found = true;
		}

		if (found == true)
			node->keyframe.push_back(frameData);

	}

	// Keyframe 늘려주기
	if (node->keyframe.size() < animation->frameCount)
	{

		uint32 count = animation->frameCount - node->keyframe.size();
		asKeyframeData keyFrame = node->keyframe.back();

		for (uint32 n = 0; n < count; n++)
			node->keyframe.push_back(keyFrame);
	}

	return node;


}
void Converter::ReadKeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asKeyFrameTemp>>& cache)
{
	shared_ptr<asKeyframe> keyframe = make_shared<asKeyframe>();
	keyframe->boneName = srcNode->mName.C_Str();

	shared_ptr<asKeyFrameTemp> findNode = cache[srcNode->mName.C_Str()];

	for (uint32 i = 0; i < animation->frameCount; i++)
	{
		asKeyframeData frameData;

		if (findNode == nullptr)
		{
			Matrix transform(srcNode->mTransformation[0]);
			transform = transform.Transpose();
			frameData.time = (float)i;
			transform.Decompose(OUT frameData.scale, OUT frameData.rotation, OUT frameData.translation);
		}

		else
		{
			frameData = findNode->keyframe[i];
		}

		keyframe->transforms.push_back(frameData);
	}

	// 애니메이션 키프레임 채우기
	animation->keyframes.push_back(keyframe);

	for (uint32 i = 0; i < srcNode->mNumChildren; i++)
		ReadKeyframeData(animation, srcNode->mChildren[i], cache);

}
;
