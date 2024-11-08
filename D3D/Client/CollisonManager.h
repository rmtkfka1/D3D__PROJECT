#pragma once

class BaseCollider;
class BoxCollider;

enum class ColliderBehave
{
	Active,
	Passive
};

class CollisonManager
{
public:


	static CollisonManager* GetInstance()
	{
		static CollisonManager instance;
		return &instance;
	}


public:
	void Init();
	void Update();
	void Clear();

	void CheckObjectCollusion();
	void CheckRayCollusion();
	bool CheckRayCollusion(Ray ray , shared_ptr<BaseCollider> collider);
	void LateUpdate();
	vec3 ToWorldPosition(const vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P);

	void ReserveDeleteCollider(shared_ptr<BaseCollider>& collider);
	void AddCollider(const shared_ptr<BaseCollider>& collider , ColliderBehave behave);

	void Reset();
private:
	void RemoveCollider(shared_ptr<BaseCollider>& collider);

private:

	vector<shared_ptr<BaseCollider>> _activeColliders;
	vector<shared_ptr<BaseCollider>> _passiveColliders;

	queue<shared_ptr<BaseCollider>> _deleteColliders;
};

