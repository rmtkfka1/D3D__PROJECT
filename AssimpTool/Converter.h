#pragma once

class Converter
{
public:
	Converter();
	~Converter();


public:
	void ReadAssetFile(wstring file);
	void ExportModelData(wstring savePath, DataType type);
	void ExportMaterialData(wstring savePath);
	void ExportAnimationData(wstring savePath, uint32 index = 0);
	void PrintAnimationData(shared_ptr<asAnimation> animation);

private:
	void ReadModelData(aiNode* node, int32 index, int32 parent, DirectX::SimpleMath::Matrix tr);
	void ReadMeshData(aiNode* node, int32 bone, DirectX::SimpleMath::Matrix m);
	void WriteModelFile(wstring finalPath);
	void CalculateBoundingBox();

private:
	void ReadMaterialData();
	void WriteMaterialData(wstring finalPath);
	void ReadSkinData();
	void WriteAnimationData(shared_ptr<asAnimation> animation, wstring finalPath);
	uint32 GetBoneIndex(const string& name);
	string WriteTexture(string saveFolder, string file);

private:
	shared_ptr<asAnimation> ReadAnimationData(aiAnimation* source);
	shared_ptr<asKeyFrameTemp> ParseAnimationNode(shared_ptr<asAnimation> animation, aiNodeAnim* srcNode);
	void ReadKeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode, map<string, shared_ptr<asKeyFrameTemp>>& cache);
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

	DataType _type;
	BoundingBox _totalbox;
	BoundingSphere _totalSphere;
};

