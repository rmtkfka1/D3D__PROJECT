#include "pch.h"
#include "Converter.h"
#include <random>

default_random_engine dre;

Converter converter;


int main()
{
	{
		converter.ReadAssetFile(L"untitled.fbx");
		converter.ExportMaterialData(L"untitled/untitled");
		converter.ExportModelData(L"untitled/untitled", DataType::STATIC);
	}


	//{
	//	converter.ReadAssetFile(L"Kachujin/Slash.fbx");
	//	converter.ExportAnimationData(L"Kachujin/Slash");
	//}




	/*for (auto& bone : converter._bones)
	{

		cout << bone->name << "," << bone->index << " " << bone->parent << endl;
	}*/
};
