#pragma once
#include "CustomObject.h"

class Texture;
class StreamOutputBuffer;
class Graphics;

struct StreamOutputData
{
	bool binit = false;
	shared_ptr<GraphicsShader> SOShader;
	shared_ptr<StreamOutputBuffer> SOBuffer;
	bool _bStreamRender = false;
};


class BilboardObject : public CustomObject
{
public:
	BilboardObject();
	virtual ~BilboardObject();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;



public:

private:
	static StreamOutputData _soData;
};

