#pragma once

class MeshRenderer;

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
	virtual void Init() =0;
	virtual void Update()=0;
	virtual void Render()=0;

private:
	GameObjectType _type;
};

