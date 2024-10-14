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

void Converter::ExportMaterialData(wstring savePath )
{
	wstring finalPath = _texturePath + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}




void Converter::ExportModelData(wstring savePath, bool useHireacy)
{
	_useHireacy = useHireacy;

	wstring finalPath = _modelPath + savePath + L".mesh";
	Matrix tr;

	ReadModelData(_scene->mRootNode, -1, -1, tr);
	WriteModelFile(finalPath);
}



void Converter::ReadModelData(aiNode* node, int32 index, int32 parent, DirectX::SimpleMath::Matrix tr )
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
	if (_useHireacy == false)
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

	if (_useHireacy == false)
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
				/*	out.write((const char*)&mesh->vertices[i].blendIndices, sizeof(vec4));
					out.write((const char*)&mesh->vertices[i].blendWeights, sizeof(vec4));*/
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
	if (_useHireacy)
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
};
