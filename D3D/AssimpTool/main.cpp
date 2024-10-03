#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"Tank/Tank.fbx");
	converter.ExportMaterialData(L"Tank/Tank");
	converter.ExportModelData(L"Tank/Tank");

	converter._meshes;
	converter._bones;
	converter._materials;

	int a = 10;
};
