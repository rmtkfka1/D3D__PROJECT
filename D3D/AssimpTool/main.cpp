#include "pch.h"
#include "Converter.h"
#include <random>
#include "AssimpTest.h"
default_random_engine dre;

Converter converter;
AssimpTest test;

int main()
{
	{
		converter.ReadAssetFile(L"ghost/source/ghost.fbx");
		//converter.ExportMaterialData(L"ghost/ghost");
		converter.ExportModelData(L"ghost/ghost", DataType::HIEARCY);
		//converter.ExportAnimationData(L"ghost/ghost");
	}

	//{
	//	converter.ReadAssetFile(L"Kachujin/Idle.fbx");
	//	converter.ExportAnimationData(L"Kachujin/Idle");

	//}

	//{
	//	converter.ReadAssetFile(L"sponge.glb");
	//	converter.ExportAnimationData(L"sponge/sponge");
	//}

	//test.Init(L"sponge.glb");

};
