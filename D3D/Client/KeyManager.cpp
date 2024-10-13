#include "pch.h"
#include "KeyManager.h"
#include "Core.h"
#include "TimeManager.h"
#include "SceneManager.h"


void KeyManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
	

	RECT _rect;
	GetWindowRect(GetForegroundWindow(), &_rect);
	_centerScreen = { (_rect.right + _rect.left) / 2, (_rect.bottom + _rect.top) / 2 };
	_mousePos = _centerScreen;

}

void KeyManager::Update()
{

	KeyUpdate();
	MouseUpdate();

}

void KeyManager::KeyUpdate()
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
}

void KeyManager::MouseUpdate()
{
	POINT _mousePos = {};
	::GetCursorPos(&_mousePos);



	// 화면 중심과의 차이 계산
	float deltaPosX = static_cast<float>(_mousePos.x - _centerScreen.x);
	float deltaPosY = static_cast<float>(_mousePos.y - _centerScreen.y);

	// Yaw와 Pitch 업데이트
	_dx = -deltaPosX  * _mouseSpeed;
	_dy = deltaPosY * _mouseSpeed;


	if (SceneManager::GetInstance()->GetSceneType() == SceneType::STAGE1)
	{
		SetCursorPos(static_cast<int>(_centerScreen.x), static_cast<int>(_centerScreen.y));
	}
}

