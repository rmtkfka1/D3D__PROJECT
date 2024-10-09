#pragma once
#include "CameraManager.h"
class GameObject;
class Shader;
class Mesh;
class Material;
class Transform;

class Scene
{

public:
	Scene();
	virtual ~Scene();

	virtual void Init();
	virtual void Run();
	virtual void LateUpdate();

	void ReserveAddGameObject(shared_ptr<GameObject> object);
	void ReserveDeleteGameObject(shared_ptr<GameObject> object);
	void AddForwardObject(shared_ptr<GameObject> object);
	void AddUiObject(shared_ptr<GameObject> object);
	void AddDeferredObject(shared_ptr<GameObject> object);
	void DeleteGameObject(shared_ptr<GameObject> object);

protected:
	queue<shared_ptr<GameObject>> _reserveAddQueue;
	queue<shared_ptr<GameObject>> _reserveDeleteQueue;

	vector<shared_ptr<GameObject>> _forwardObjects;
	vector<shared_ptr<GameObject>> _uiObjects;
	vector<shared_ptr<GameObject>> _deferredObjects;
private:

	void DeferredRender();
	void ForwardRender();
	void UiObjectRender();
};

