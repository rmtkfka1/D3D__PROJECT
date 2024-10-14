#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"earth/source/Earth.fbx");
	//converter.ExportMaterialData(L"earth/earth");
	converter.ExportModelData(L"earth/earth",false);

	for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}
};
