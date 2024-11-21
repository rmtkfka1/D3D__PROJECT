#pragma once
#include "ResourceBase.h"
class GraphicsShader;
class Texture;

struct MaterialParams
{
	void SetInt(uint8 index, int32 value) { intParams[index] = value; }
	void Setfloat(uint8 index, float value) { floatParams[index] = value; }
	void SetTexon(uint8 index, int value) { TextureParams[index] = value; }

	array<int32, 4> intParams{0};
	array<float, 4> floatParams{0};
	array<int, 4> TextureParams{0};
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

	void PushData();
	void PushComputeData();


private:
	shared_ptr<Texture> _diffuseTexture;
	shared_ptr<Texture> _normalTexture;
	shared_ptr<Texture> _SpecularTexture;

private:
	MaterialParams		_params;
};

