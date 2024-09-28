#pragma once
class Game
{
public:
	void Init(HWND hwnd);
	void Run();

	void Update();
	void Render();

private:
	HWND _hwnd;
};

