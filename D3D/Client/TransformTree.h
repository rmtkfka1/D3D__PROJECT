#pragma once

class Transform;
class Model;

class TransformTree
{
public:
	TransformTree();
	virtual~TransformTree();

	void Update();
public:
	void MakeTransformTree(shared_ptr<Model> model);
public:
	shared_ptr<Transform>& findByName(wstring name);

public:
	shared_ptr<Transform>& GetRoot() { return _root; }

private:
	shared_ptr<Transform> _root;
	unordered_map<wstring, shared_ptr<Transform>> _nameMap;

};

