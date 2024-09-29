#include "pch.h"
#include "KeyManager.h"
#include "Core.h"




void KeyManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void KeyManager::Update()
{

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};

	if (::GetKeyboardState(asciiKeys) == false)
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{

		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];


			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}

		else
		{
			KEY_STATE& state = _states[key];

			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::UP;
		}
	}


	::GetCursorPos(&_mousePos);
	//::ScreenToClient(core->GetWindowInfo().hwnd, &_mousePos);

}

