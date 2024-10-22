#include "pch.h"
#include "Material.h"
#include "core.h"
#include "BufferPool.h"
#include "Texture.h"
#include "Shader.h"
#include "GameObject.h"

Material::Material():ResourceBase(ResourceType::Material)
{


}
Material::~Material()
{
}



void Material::Pushdata()
{

	if (_diffuseTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0));
		core->GetTableHeap()->CopySRV(_diffuseTexture->GetCpuHandle(), reg);
		_params.SetTexon(0, 1);
	}
	else
	{
		_params.SetTexon(0, 0);
	}

	if (_normalTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t1));
		core->GetTableHeap()->CopySRV(_normalTexture->GetCpuHandle(), reg);
		_params.SetTexon(1, 1);
	}
	else
	{
		_params.SetTexon(1, 0);
	}
	
	if (_SpecularTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t2));
		core->GetTableHeap()->CopySRV(_SpecularTexture->GetCpuHandle(), reg);
		_params.SetTexon(2, 1);
	}
	else
	{
		_params.SetTexon(2, 0);
	}


	core->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
	
}
