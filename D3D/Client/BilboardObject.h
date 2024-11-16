#pragma once
#include "CustomObject.h"

class Texture;
class StreamOutputBuffer;
class Shader;
class BilboardObject :	public CustomObject
{
public:
	BilboardObject();
	virtual ~BilboardObject();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;



public:

	bool _useWithHeightMap = false;


private:
	static bool _streamRender;
	shared_ptr<Shader> _SOShader;
	
};

