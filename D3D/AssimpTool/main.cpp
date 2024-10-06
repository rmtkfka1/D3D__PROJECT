#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"helicopter/source/Bell Huey.fbx");
	//converter.ExportMaterialData(L"helicopter/helicopter");
	converter.ExportModelData(L"helicopter/helicopter",true);

	for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}
};
