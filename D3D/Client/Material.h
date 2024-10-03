#pragma once
#include "ResourceBase.h"
class Shader;
class Texture;

struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void Setfloat(uint8 index, float value) { floatParams[index] = value; }
	void SetMatrix(uint8 index, Matrix& value) { matrixParams[index] = value; }


	array<int32, 4> intParams{};
	array<float, 4> floatParams{};
	array<Matrix, 1> matrixParams{};
};


class Material :public ResourceBase
{
public:
	Material();
	virtual ~Material();

	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.Setfloat(index, value); }
	void SetMatrix(uint8 index, Matrix& value) { _params.SetMatrix(index, value); }

	void SetDiffuseTexture(shared_ptr<Texture> texture) { _diffuseTexture = texture; }
	void SetNormalTexture(shared_ptr<Texture> texture) { _normalTexture = texture; }
	void SetSpecularTexture(shared_ptr<Texture> texture) { _SpecularTexture = texture; }

	void SetShader(shared_ptr<Shader> shader) { _shader = shader; }
	void Pushdata();

private:
	shared_ptr<Texture> _diffuseTexture;
	shared_ptr<Texture> _normalTexture;
	shared_ptr<Texture> _SpecularTexture;
	shared_ptr<Shader> _shader;

public:
	MaterialParams		_params;

};

