#pragma once

class MeshRenderer;
class Transform;
class BaseCollider;

enum class GameObjectType
{
	Custom,
	Model,
	Hierarchy
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
private:
	GameObjectType _type;
};

