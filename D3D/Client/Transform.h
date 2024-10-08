#pragma once


class Transform 
{
public:
	Transform();
	~Transform();


	void Update();
	void PushData();

	void AddMove(const vec3& shift);
	void AddRotate(const vec3& Euler);
	void AddRotate(const Quaternion& quat);
	// Local
	vec3 GetLocalScale() { return _localScale; }
	void SetLocalScale(const vec3 localScale) { _localScale = localScale; }

	Quaternion GetLocalRotation() { return _localRotation; }
	void SetLocalRotation(const Quaternion localRotation) { _localRotation = localRotation; }
	void SetLocalRotation(const vec3 localRotation);

	vec3 GetLocalPosition() { return _localPosition; }
	void SetLocalPosition(const vec3 localPosition) { _localPosition = localPosition; }

	// World
	vec3 GetWorldScale() { return _scale; }
	void SetWorldScale(const vec3 scale);
	Quaternion GetWorldRotation() { return _rotation; }
	void SetWorldRotation(const vec3 rotation);
	vec3 GetWorldPosition() { return _position; }
	void SetWorldPosition(const vec3 position);

	vec3 GetRight();
	vec3 GetUp();
	vec3 GetLook();

	void SetCenter(vec3 center) { _center = center; }
	void SetSize(vec3 size) { _size = size; }

	Matrix GetWorldMatrix() { return _matWorld; }

	bool HasParent() { return _parent.lock() != nullptr; }

	shared_ptr<Transform> GetParent() { return _parent.lock(); }
	void SetParent(shared_ptr<Transform> parent) { _parent = parent; }

	const vector<shared_ptr<Transform>>& GetChildren() { return _children; }
	void AddChild(shared_ptr<Transform> child) { _children.push_back(child); }

	wstring _name{};
private:
	vec3 _localScale = { 1.f, 1.f, 1.f };
	Quaternion _localRotation;
	vec3 _localPosition = { 0.f, 0.f, 0.f };


	Matrix _matLocal = Matrix::Identity;
	Matrix _matWorld = Matrix::Identity;

	vec3 _scale;
	Quaternion _rotation;

	float _yaw =0;
	float _pitch = 0;
	float _roll = 0;

	vec3 _position;

	vec3 _right;
	vec3 _up;
	vec3 _look;

	vec3 _center{};
	vec3 _size{};
	float _totalRadius{};

private:
	weak_ptr<Transform> _parent;
	vector<shared_ptr<Transform>> _children;
};

