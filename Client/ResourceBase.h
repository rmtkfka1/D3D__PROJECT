#pragma once

enum class ResourceType : int8
{
	Mesh,
	Model,
	Shader,
	Texture,
	Material,
	End
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<int8>(ResourceType::End)
};

class ResourceBase
{
public:
	friend class ResourceManager;

	ResourceBase(ResourceType type);
	virtual ~ResourceBase();

	ResourceType GetType() { return _type; }
	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }


protected:
	ResourceType _type = ResourceType::Mesh;
	wstring _name;
	wstring _path;
};
