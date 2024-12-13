#include "pch.h"
#include "Mesh.h"
#include "Model.h"
#include "tinyxml2.h"
#include "Utils.h"
#include "Material.h"
#include "ResourceManager.h"
#include <fstream>
#include <iostream>
#include "Texture.h"
#include "FileUtils.h"

Model::Model() :ResourceBase(ResourceType::Model)
{

}

Model::~Model()
{

}

shared_ptr<Model> Model::ReadData(wstring filename, wstring key)
{

	shared_ptr<Model> model = ResourceManager::GetInstance()->Get<Model>(filename);

	if (model)
	{
		return model;
	};

	model = make_shared<Model>();
	model->SetName(filename);
	model->ReadMaterial(filename);
	model->ReadModel(filename);

	ResourceManager::GetInstance()->Add<Model>(key, model);

	return model;
}



void Model::ReadMaterial(wstring filename)
{
	wstring fullPath = _texturePath + filename + L".xml";
	auto parentPath = filesystem::path(fullPath).parent_path();
	auto lastname = parentPath.filename();

	std::unique_ptr<tinyxml2::XMLDocument> document = std::make_unique<tinyxml2::XMLDocument>();
	tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = document->FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{
		shared_ptr<Material> material = make_shared<Material>();

		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		material->SetName(Utils::ToWString(node->GetText()));

		// Diffuse Texture
		node = node->NextSiblingElement();

		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				auto texture = ResourceManager::GetInstance()->Load<Texture>((lastname / textureStr).wstring());
				texture->SetName(textureStr);
				material->SetDiffuseTexture(texture);
			}
		}

		// Normal Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				auto texture = ResourceManager::GetInstance()->Load<Texture>((lastname / textureStr).wstring());
				texture->SetName(textureStr);
				material->SetNormalTexture(texture);
			}
		}


		// Specular Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring texture = Utils::ToWString(node->GetText());
			if (texture.length() > 0)
			{
				wstring textureStr = Utils::ToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					auto texture = ResourceManager::GetInstance()->Load<Texture>((lastname / textureStr).wstring());
					texture->SetName(textureStr);
					material->SetSpecularTexture(texture);
				}
			}
		}

		// Ambient
		{
			node = node->NextSiblingElement();

			vec4 _color;
			_color.x = node->FloatAttribute("R");
			_color.y = node->FloatAttribute("G");
			_color.z = node->FloatAttribute("B");
			_color.w = node->FloatAttribute("A");
			///*		material->GetMaterialDesc().ambient = _color;*/
		}

		// Diffuse
		{
			node = node->NextSiblingElement();

			vec4 _color;
			_color.x = node->FloatAttribute("R");
			_color.y = node->FloatAttribute("G");
			_color.z = node->FloatAttribute("B");
			_color.w = node->FloatAttribute("A");
			/*	material->GetMaterialDesc().diffuse = _color;*/
		}

		// Specular
		{
			node = node->NextSiblingElement();

			vec4 _color;
			_color.x = node->FloatAttribute("R");
			_color.y = node->FloatAttribute("G");
			_color.z = node->FloatAttribute("B");
			_color.w = node->FloatAttribute("A");
			//material->GetMaterialDesc().specular = _color;
		}

		// Emissive
		{
			node = node->NextSiblingElement();

			vec4 _color;
			_color.x = node->FloatAttribute("R");
			_color.y = node->FloatAttribute("G");
			_color.z = node->FloatAttribute("B");
			_color.w = node->FloatAttribute("A");
			/*	material->GetMaterialDesc().emissive = _color;*/
		}

		_materialData.push_back(material);

		// Next Material
		materialNode = materialNode->NextSiblingElement();
	}

	BindCacheInfo();
}

void Model::ReadModel(wstring filename)
{
	wstring fullPath = _modelPath + filename + L".mesh";

	ifstream in{ fullPath ,ios::binary };

	assert(in);

	//bone ���� �б�
	{
		int count;
		in.read((char*)&count, sizeof(int));
		_boneData.reserve(count);

		for (int i = 0; i < count; ++i)
		{
			shared_ptr<ModelBone> newbone = make_shared<ModelBone>();

			{
				//bone->name
				size_t len{};
				in.read((char*)&len, sizeof(size_t));

				string temp;
				temp.resize(len);
				in.read((char*)temp.data(), len);

				newbone->name = wstring{ temp.begin(),temp.end() };
			}

			{
				//index , parent index
				in.read((char*)&newbone->index, sizeof(int32));
				in.read((char*)&newbone->parentIndex, sizeof(int32));
			}

			{
				//��Ʈ��������
				XMFLOAT4X4 tempData;
				in.read((char*)(&tempData), sizeof(XMFLOAT4X4));
				newbone->transformData = XMLoadFloat4x4(&tempData);
			}

			_boneData.push_back(newbone);
		}

	};


	//mesh �����б�

	{
		int count;
		in.read((char*)&count, sizeof(int));
		_meshData.reserve(count);

		for (int i = 0; i < count; ++i)
		{
			shared_ptr<ModelMesh> newMesh = make_shared<ModelMesh>();
			newMesh->meshes = make_shared<Mesh>();

			{
				//name �б�
				size_t len;
				in.read((char*)&len, sizeof(size_t));

				string temp;
				temp.resize(len);
				in.read((char*)temp.data(), len);

				newMesh->name = wstring{ temp.begin(), temp.end() };
			}

			{
				//boneindex �б�
				in.read((char*)&newMesh->boneIndex, sizeof(int32));
			}

			{
				//materialnmae �б�
				size_t len;
				in.read((char*)&len, sizeof(size_t));

				string temp;
				temp.resize(len);
				in.read((char*)temp.data(), len);

				newMesh->materialName = wstring{ temp.begin(), temp.end() };

			}

			{
				//vertices �б�
				size_t len;
				in.read((char*)&len, sizeof(size_t));

				vector<Vertex> vertices;
				vertices.resize(len);

				for (int j = 0; j < len; ++j)
				{
					in.read((char*)&vertices[j].position, sizeof(vec3));
					in.read((char*)&vertices[j].uv, sizeof(vec2));
					in.read((char*)&vertices[j].normal, sizeof(vec3));
					in.read((char*)&vertices[j].tangent, sizeof(vec3));
					in.read((char*)&vertices[j].blendIndices, sizeof(vec4));
					in.read((char*)&vertices[j].blendWeights, sizeof(vec4));
				}



				//indices �б�
				uint32 len2;
				in.read((char*)&len2, sizeof(uint32));
				vector<uint32> indicies;
				indicies.resize(len2);

				for (uint32 j = 0; j < len2; ++j)
				{
					in.read((char*)&indicies[j], sizeof(uint32));
				}


				{
					in.read((char*)&newMesh->box.Center, sizeof(vec3));
					in.read((char*)&newMesh->box.Extents, sizeof(vec3));
				}

				//mesh �����
				newMesh->meshes->Init(vertices, indicies);
				_meshData.push_back(newMesh);
			}

		}
	}

	{

		in.read((char*)&_totalCenter, sizeof(vec3));
		in.read((char*)&_totalSize, sizeof(vec3));
		in.read((char*)&_totalRadius, sizeof(float));
	}

	BindCacheInfo();
}

void Model::ReadAnimation(wstring filename)
{
	wstring fullPath = _modelPath + filename + L".clip";

	//shared_ptr<FileUtils> file = make_shared<FileUtils>();
	//file->Open(fullPath, FileMode::Read);
	ifstream in{ fullPath ,ios::binary };

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	size_t nameLen;
	in.read((char*)&nameLen, sizeof(size_t));

	string temp;
	temp.resize(nameLen);
	in.read((char*)temp.data(), nameLen);

	animation->name = wstring{ temp.begin(), temp.end() };

	in.read((char*)&animation->frameRate, sizeof(float));
	in.read((char*)&animation->frameCount, sizeof(uint32));

	size_t keyframeCount;
	in.read((char*)&keyframeCount, sizeof(size_t));

	for (int i = 0; i < keyframeCount; ++i)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();

		size_t boneNamelen;
		in.read((char*)&boneNamelen, sizeof(size_t));

		string temp;
		temp.resize(boneNamelen);
		in.read((char*)temp.data(), boneNamelen);

		keyframe->boneName= wstring{ temp.begin(), temp.end() };
	
		size_t bonesize;
		in.read((char*)&bonesize, sizeof(size_t));

		if (bonesize > 0)
		{
			keyframe->transforms.resize(bonesize);
		}

		for (int i = 0; i < bonesize; ++i)
		{
			in.read((char*)&keyframe->transforms[i].time, sizeof(float));
			in.read((char*)&keyframe->transforms[i].scale, sizeof(vec3));
			in.read((char*)&keyframe->transforms[i].rotation, sizeof(Quaternion));
			in.read((char*)&keyframe->transforms[i].translation, sizeof(vec3));
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}


	_animations = animation;
}

void Model::SetIntValue(uint8 index, int32 value)
{

	for (auto& ele : _materialData)
	{
		ele->SetInt(index, value);
	}

}

void Model::SetFloatValue(uint8 index, float value)
{
	for (auto& ele : _materialData)
	{
		ele->SetFloat(index, value);
	}
}





void Model::PrintInfo()
{
	cout << "������: " << _totalSize.x << " " << _totalSize.y << " " << _totalSize.z << endl;
	cout << "�߽���: " << _totalCenter.x << " " << _totalCenter.y << " " << _totalCenter.z << endl;

	cout << endl;
}

shared_ptr<Material> Model::GetMaterialByName(const wstring& name)
{
	for (auto& material : _materialData)
	{
		if (material->GetName() == name)
			return material;
	}

	return nullptr;
}

std::shared_ptr<ModelMesh> Model::GetMeshByName(const wstring& name)
{
	for (auto& mesh : _meshData)
	{
		if (mesh->name == name)
			return mesh;
	}

	return nullptr;
}

std::shared_ptr<ModelBone> Model::GetBoneByName(const wstring& name)
{
	for (auto& bone : _boneData)
	{
		if (bone->name == name)
			return bone;
	}

	return nullptr;
}

shared_ptr<ModelAnimation> Model::GetAnimationByName(wstring name)
{
	//for (auto& animation : _animations)
	//{
	//	if (animation->name == name)
	//		return animation;
	//}

	return nullptr;
}




void Model::BindCacheInfo()
{
	// Mesh�� Material ĳ��
	for (const auto& mesh : _meshData)
	{
		// �̹� ã������ ��ŵ
		if (mesh->material != nullptr)
			continue;

		mesh->material = GetMaterialByName(mesh->materialName);
	}

	// Mesh�� Bone ĳ��
	for (const auto& mesh : _meshData)
	{
		// �̹� ã������ ��ŵ
		if (mesh->bone != nullptr)
			continue;

		mesh->bone = GetBoneByIndex(mesh->boneIndex);
	}

	// Bone ���� ���� ä���
	if (_root == nullptr && _boneData.size() > 0)
	{
		_root = _boneData[0];

	}
}

