#pragma once

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',
	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',
	O = 'O',
	ONE = '1',
	TWO = '2',
	THREE = '3',
	SPACE = VK_SPACE,
	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,
	ENTER = VK_RETURN

};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class KeyManager
{
public:

	static KeyManager* GetInstance()
	{
		static KeyManager instance;
		return &instance;
	}

	void Init(HWND hwnd);
	void Update();
	void SetCenterPos(POINT point) { _centerScreen = point; }
	void KeyUpdate();
	void MouseUpdate();

	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	const POINT& GetMousePos() { return _mousePos; }
	vec2 GetDeletaPos() { return vec2{ _dx,_dy }; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND _hwnd{};
	vector<KEY_STATE> _states;

	POINT _mousePos = {};
	POINT _centerScreen{};

	float _mouseSpeed = 0.1f;

private:
	float _dx{};
	float _dy{};
};

