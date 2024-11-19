#pragma once
#include "CameraManager.h"
class GameObject;
class GraphicsShader;
class Mesh;
class Material;
class Transform;


enum class RenderingType
{
	Deferred,
	Forward,
	Ui
};

class Scene
{

public:
	Scene();
	virtual ~Scene();

	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void LateUpdate() = 0;

	void ReserveAddGameObject(const shared_ptr<GameObject>& object, RenderingType type);
	void ReserveDeleteGameObject(const shared_ptr<GameObject>& object, RenderingType type);

	void DeleteGameObject(const shared_ptr<GameObject>& object, RenderingType type);
	void AddGameObject(const shared_ptr<GameObject>& object, RenderingType type);


protected:
	queue<pair<shared_ptr<GameObject>,RenderingType>> _reserveAddQueue;
	queue<pair<shared_ptr<GameObject>,RenderingType>> _reserveDeleteQueue;

	vector<shared_ptr<GameObject>> _forwardObjects;
	vector<shared_ptr<GameObject>> _uiObjects;
	vector<shared_ptr<GameObject>> _deferredObjects;

	
};

