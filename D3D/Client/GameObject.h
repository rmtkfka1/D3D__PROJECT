#pragma once
#include "BaseCollider.h"
class MeshRenderer;
class Transform;
class BaseCollider;
class Shader;

enum class GameObjectType
{
	Custom,
	Model,
	Hierarchy
};

enum class PlayerType
{
	Default,
	Player,
	Box,
	Sphere,
	Sea,
	Terrain
};

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject(GameObjectType type);
	virtual ~GameObject();
public:

	virtual shared_ptr<Transform> GetTransform()=0;
	virtual void Init() =0;
	virtual void Update()=0;
	virtual void Render()=0;

	virtual void OnComponentBeginOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider>other) = 0;
	virtual void OnComponentEndOverlap(shared_ptr<BaseCollider> collider, shared_ptr<BaseCollider> other) = 0;

	virtual void AddCollider(string name, ColliderType type, vec3 offsetSize = vec3(0, 0, 0), vec3 offsetCeneter = vec3(0, 0, 0)) =0;
	virtual void AddBoxCollider(string name, vec3 size, vec3 center) =0;

	void BoundingRender();

	shared_ptr<BaseCollider>& GetCollider();
	GameObjectType GetGameObjectType() { return _type; }
	PlayerType GetPlayerType() { return _playerType; }

	void SetFrustumCuling(bool checkFrustum) { _checkFrustum = checkFrustum; }
	bool GetFrustumCuling() { return _checkFrustum; }
	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }

protected:
	GameObjectType _type;
	PlayerType _playerType =PlayerType::Default;
	vector<shared_ptr<BaseCollider>> _colliders;

	shared_ptr<Shader> _shader;
	bool _checkFrustum = true;

};

