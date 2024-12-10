#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	{
		converter.ReadAssetFile(L"Kachujin/Mesh.fbx");
		converter.ExportMaterialData(L"Kachujin/Kachujin");
		converter.ExportModelData(L"Kachujin/Kachujin", DataType::ANIMATION);
	}

	{
		converter.ReadAssetFile(L"Kachujin/Idle.fbx");
		converter.ExportAnimationData(L"Kachujin/Idle");
	}
};
