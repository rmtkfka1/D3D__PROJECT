#include "pch.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "LightManager.h"
#include "Material.h"
#include "Animator.h"

BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
}

void BufferManager::Init()
{

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_table[i] = make_shared<DescriptorTable>();
		_table[i]->Init(10000);
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_CameraBufferPool[i] = make_shared<ConstantBufferPool>();
		_CameraBufferPool[i]->Init(CBV_REGISTER::None, sizeof(CameraParams), 50); //b2 는 계산에 이용되지않음
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_lightBufferPool[i] = make_shared<ConstantBufferPool>();
		_lightBufferPool[i]->Init(CBV_REGISTER::None, sizeof(LightParams), 1); //b2 는 계산에 이용되지않음
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_WorldBufferPool[i] = make_shared<ConstantBufferPool>();
		_WorldBufferPool[i]->Init(CBV_REGISTER::b2, sizeof(TransformParams), 255);
	}


	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_materialParamsBufferPool[i] = make_shared<ConstantBufferPool>();
		_materialParamsBufferPool[i]->Init(CBV_REGISTER::b3, sizeof(MaterialParams), 1000); //b0 는 계산에 이용되지않음
	}

	for (int i = 0; i < MAX_FRAME_COUNT; ++i)
	{
		_aniMationBufferPool[i] = make_shared<ConstantBufferPool>();
		_aniMationBufferPool[i]->Init(CBV_REGISTER::None, sizeof(Matrix)*MAX_BONE , 1); 
	}

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(255,SWAP_CHAIN_FRAME_COUNT + 5,5);



}

void BufferManager::Clear(int nextValue)
{
	_WorldBufferPool[nextValue]->Clear();
	_CameraBufferPool[nextValue]->Clear();
	_table[nextValue]->Clear();
	_materialParamsBufferPool[nextValue]->Clear();
}

