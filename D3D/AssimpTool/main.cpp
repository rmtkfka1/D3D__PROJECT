#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"comical-bomb/source/ComBomb_ForSket.fbx");
	converter.ExportMaterialData(L"bomb/bomb");
	converter.ExportModelData(L"bomb/bomb",false);

	for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}
};
