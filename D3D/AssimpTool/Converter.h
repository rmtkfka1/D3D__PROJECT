#pragma once



class Converter
{
public:
	Converter();
	~Converter();


public:
	void ReadAssetFile(wstring file);
	void ExportModelData(wstring savePath, bool useHireacy);
	void ExportMaterialData(wstring savePath);


private:

	void ReadModelData(aiNode* node, int32 index, int32 parent, DirectX::SimpleMath::Matrix tr );
	void ReadMeshData(aiNode* node, int32 bone, DirectX::SimpleMath::Matrix m);
	void WriteModelFile(wstring finalPath);
	void CalculateBoundingBox();

private:

	void ReadMaterialData();
	void WriteMaterialData(wstring finalPath);
	string WriteTexture(string saveFolder, string file);

private:
	wstring _assetPath = L"../Resources/Asset/";
	wstring _modelPath = L"../Resources/Model/";
	wstring _texturePath = L"../Resources/Texture/";

private:
	shared_ptr<Assimp::Importer> _importer;
	const aiScene* _scene;

public:
	vector<shared_ptr<asBone>> _bones;
	vector<shared_ptr<asMesh>> _meshes;
	vector<shared_ptr<asMaterial>> _materials;

private:
	bool _useHireacy;
	BoundingBox _totalbox;
	BoundingSphere _totalSphere;
};

