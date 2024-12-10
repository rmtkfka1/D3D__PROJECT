#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	converter.ReadAssetFile(L"Mesh.fbx");
	converter.ExportMaterialData(L"x/x");
	converter.ExportModelData(L"x/x",DataType::HIEARCY);

	for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}
};
