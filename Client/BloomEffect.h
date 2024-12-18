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

	void DepthRender();
	void Render(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void SetInt(uint8 index, int32 value) { _params.SetInt(index, value); }
	void SetFloat(uint8 index, float value) { _params.Setfloat(index, value); }

private:
	void Bloom(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void Blurring(int32 disPatchX, int32 disPatchY, int32 disPatchZ);

	void BlurringProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void BlackProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ);
	void BloomProcess(int32 disPatchX, int32 disPatchY, int32 disPatchZ);

private:
	shared_ptr<ComputeShader>		_xblurShader;
	shared_ptr<ComputeShader>		_yblurShader;
	shared_ptr<ComputeShader>       _blackShader;
	shared_ptr<ComputeShader>       _bloomShader;
	
	shared_ptr<Texture>             _texture; 
	shared_ptr<Texture>             _texture2;
	shared_ptr<Texture>             _bloomTexture;
	shared_ptr<Texture>				_interMediateTexture;

	MaterialParams					_params;
};

