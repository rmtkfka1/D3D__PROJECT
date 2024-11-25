#pragma once

#include "Material.h"

class Texture;
class ComputeShader;

class BloomEffect
{

public:

	BloomEffect();
	~BloomEffect();

	void GenTexture();

	void Render(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void PingPongRender(int32 disPatchX, int32 disPatchY, int32 disPatchZ);

	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.Setfloat(index, value); }

private:
	void PostProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void JustRender();


private:
	shared_ptr<ComputeShader>		_xblurShader;
	shared_ptr<ComputeShader>		_yblurShader;
	MaterialParams					_params;
	
	shared_ptr<Texture>             _texture;
	shared_ptr<Texture>             _texture2;
	shared_ptr<Texture>				_interMediateTexture;
};

