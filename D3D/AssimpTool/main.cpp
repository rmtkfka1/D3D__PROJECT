#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"Tank/Tank.fbx");
	//converter.ExportMaterialData(L"helicopter/helicopter");
	converter.ExportModelData(L"Tank/Tank",true);

	for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}
};
