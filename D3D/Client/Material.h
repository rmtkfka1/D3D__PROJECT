#pragma once
#include "ResourceBase.h"
class Shader;
class Texture;

struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value;  useParams = 1;}
	void Setfloat(uint8 index, float value) { floatParams[index] = value; useParams = 1; }


	array<int32, 2> intParams{};
	array<float, 1> floatParams{};
	int useParams = 0;
};


class Material :public ResourceBase
{
public:
	Material();
	virtual ~Material();

	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.Setfloat(index, value); }

	void SetDiffuseTexture(shared_ptr<Texture> texture) { _diffuseTexture = texture; }
	void SetNormalTexture(shared_ptr<Texture> texture) { _normalTexture = texture; }
	void SetSpecularTexture(shared_ptr<Texture> texture) { _SpecularTexture = texture; }

	void Pushdata();

private:
	shared_ptr<Texture> _diffuseTexture;
	shared_ptr<Texture> _normalTexture;
	shared_ptr<Texture> _SpecularTexture;


private:
	MaterialParams		_params;
};

