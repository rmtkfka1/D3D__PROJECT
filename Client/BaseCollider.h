#pragma once

class GameObject;

enum class ColliderType
{
	Box,
	Sphere,
};

class BaseCollider
{
public:
	BaseCollider(ColliderType colliderType);
	virtual ~BaseCollider();

public:

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	virtual bool CheckCollusion(shared_ptr<BaseCollider>& other) = 0;
	virtual bool CheckCollusion(Ray& ray, OUT float& distance) = 0;	//위치,나가는방향
	void Clear();
	void Delete(BaseCollider* del);

public:
	string GetName() { return _name; }
	void SetName(string& name) { _name = std::move(name); }
	ColliderType GetColliderType() { return _colliderType; }
	void SetTotalCenter(vec3 center) { _totalCenter = center; }
	vec3 GetCenter() { return _totalCenter; }
	void SetOwner(shared_ptr<GameObject> owner) { _owner = owner; }
	shared_ptr<GameObject> GetOwner() { return _owner.lock(); }

protected:
	vec3 _totalCenter;
	ColliderType _colliderType = {};

public:
	string _name;
	weak_ptr<GameObject> _owner;
	unordered_set<BaseCollider*> _collisionMap;
	static bool brender;
};

