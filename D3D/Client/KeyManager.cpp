#include "pch.h"
#include "KeyManager.h"
#include "Core.h"
#include "TimeManager.h"



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
	//::ScreenToClient(core->GetWindowInfo().hwnd, &_mousePos);

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
	// �ð� ���� ���
	float dt = TimeManager::GetInstance()->GetDeltaTime();

	::GetCursorPos(&_mousePos);

	// ȭ�� �߽ɰ��� ���� ���
	float deltaPosX = static_cast<float>(_mousePos.x - _centerScreen.x);
	float deltaPosY = static_cast<float>(_mousePos.y - _centerScreen.y);


	// Yaw�� Pitch ������Ʈ
	_dx += deltaPosX * 30.0f * dt;  // Yaw�� ���콺 X�� �����ӿ� ����
	_dy += deltaPosY * 30.0f * dt;  // Pitch�� ���콺 Y�� �����ӿ� ���� (���� ����)


	if (_dx < 0.0f) _dx += 360.0f;
	if (_dx > 360.0f) _dx -= 360.0f;

	// Yaw �� ���� (360�� ȸ��)
	if (_dy > 89.0f) _dy = 89.0f;
	if (_dy < -89.0f) _dy = -89.0f;

	// ���콺 Ŀ���� ȭ�� �߽����� �̵�
	SetCursorPos(static_cast<int>(_centerScreen.x), static_cast<int>(_centerScreen.y));

}

