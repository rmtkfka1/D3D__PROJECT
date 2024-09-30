#pragma once
#include "CustomObject.h"


class Terrain :public CustomObject
{
public:

	Terrain();
	virtual ~Terrain();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:
	void CreateMesh();
	void LoadFIle(LPCTSTR pFileName, vec3 scale);


private:
	BYTE* _HeightMapPixels;

	vec3 _scale;
	int _width;
	int _length;
};

