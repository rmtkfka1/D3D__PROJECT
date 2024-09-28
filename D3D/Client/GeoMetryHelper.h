#pragma once

class Mesh;

class GeoMetryHelper
{
public:
	static shared_ptr<Mesh> LoadRectangleBox(const float scale = 1.0f);
	static shared_ptr<Mesh> LoadRectangleMesh(const float scale = 1.0f);
};

