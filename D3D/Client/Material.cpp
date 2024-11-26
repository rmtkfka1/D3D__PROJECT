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



void Material::PushData()
{

	if (_diffuseTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0));
		core->GetBufferManager()->GetTable()->CopySRV(_diffuseTexture->GetSRVCpuHandle(), reg);
		_params.SetTexon(0, 1);
	}
	else
	{
		_params.SetTexon(0, 0);
	}

	if (_normalTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t1));
		core->GetBufferManager()->GetTable()->CopySRV(_normalTexture->GetSRVCpuHandle(), reg);
		_params.SetTexon(1, 1);
	}
	else
	{
		_params.SetTexon(1, 0);
	}
	
	if (_SpecularTexture)
	{
		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t2));
		core->GetBufferManager()->GetTable()->CopySRV(_SpecularTexture->GetSRVCpuHandle(), reg);
		_params.SetTexon(2, 1);
	}
	else
	{
		_params.SetTexon(2, 0);
	}


	core->GetBufferManager()->GetMaterialParamsBufferPool()->PushData(&_params, sizeof(_params));
	
}







