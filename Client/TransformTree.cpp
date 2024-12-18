#include "pch.h"
#include "TransformTree.h"
#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include <unordered_map>
#include <string>
#include "Utils.h"
TransformTree::TransformTree() 
{

}

TransformTree::~TransformTree()
{
}

void TransformTree::Update()
{

	for (auto& ele : _nameMap)
	{
		ele.second->Update();
	}

}

void TransformTree::MakeTransformTree(shared_ptr<Model> model)
{
	vector<shared_ptr<ModelBone>>& bones = model->GetBones();
	unordered_map<int32, shared_ptr<Transform>> map;

	for (auto& bone : bones)
	{
		if (bone->index == -1)
		{  
			_root = make_shared<Transform>();

			Matrix m = bone->transformData; //본공간으로 변환하는 데이터

			vec3 pos;
			Quaternion rotation;
			vec3 scale;
			m.Decompose(scale, rotation, pos);

			_root->SetLocalPosition(pos);
			_root->SetLocalRotation(Utils::ToEulerAngles(rotation));
			_root->SetLocalScale(scale);
			_root->Update();
			_root->_name = bone->name;
			map[bone->index] = _root;
			_nameMap[_root->_name] = _root;
		}

		else
		{
			shared_ptr<Transform> transform = make_shared<Transform>();

			Matrix m = bone->transformData; //본공간으로 변환하는 데이터

			vec3 pos;
			Quaternion rotation;
			vec3 scale;
			m.Decompose(scale, rotation, pos);

			transform->SetLocalPosition(pos);
			transform->SetLocalRotation(Utils::ToEulerAngles(rotation));
			transform->SetLocalScale(scale);
			transform->Update();
			transform->_name = bone->name;

			transform->SetParent(map[bone->parentIndex]);
			transform->GetParent()->AddChild(transform);
			map[bone->index] = transform;
			_nameMap[transform->_name] = transform;
		}
	}

	vector<shared_ptr<ModelMesh>>& meshes = model->GetMeshes();

	for (auto& data : meshes)
	{
		shared_ptr<Transform> transform = _nameMap[data->name];

		if (transform)
			transform->SetCenter(data->box.Center);

		if (transform)
			transform->SetSize(data->box.Extents);

	}

}


shared_ptr<Transform>& TransformTree::findByName(wstring name)
{
	auto it = _nameMap.find(name);

	if (it != _nameMap.end()) {
		return it->second;
	}

	assert(false);
}
