#include "pch.h"
#include "Core.h"
#include "RootSignature.h"
#include "RenderTargets.h"
#include "D3D12ResourceManager.h"
#include "BufferPool.h"
#include "KeyManager.h"
#include "LightManager.h"
#include "ResourceManager.h"
#include "Material.h"
#include "SteamOutputBuffer.h"
#include "Graphics.h"



Core::Core()
{
};

Core::~Core()
{

};

void Core::Init(HWND hwnd, bool EnableDebugLayer, bool EnableGBV)
{
	_hwnd = hwnd;
	_graphics = make_shared<Graphics>();
	_graphics->Init(hwnd, EnableDebugLayer, EnableGBV);

};







