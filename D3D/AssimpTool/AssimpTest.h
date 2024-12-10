#pragma once
class AssimpTest
{


public:
	AssimpTest();
	~AssimpTest();

	void Init(const wstring& path);
	void ProcessMesh(aiNode* node);
	void ProcessAni();
private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

	wstring _assetPath = L"../Resources/Asset/";
	wstring _modelPath = L"../Resources/Model/";
	wstring _texturePath = L"../Resources/Texture/";

};

