#pragma once

class MeshRenderer;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

	void Push(shared_ptr<MeshRenderer> object) { _meshRenderer.push_back(object); }

protected:
	vector<shared_ptr<MeshRenderer>> _meshRenderer;
};

