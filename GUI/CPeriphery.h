#pragma once

#include <Windows.h>
#define GAMEWINDOW "Counter-Strike: Global Offensive"

enum INPUTS
{
	LBUTTON_DOWN,
	LBUTTON_CLICKED,
	KEY_INSERT_PRESSED,
	MAX_INPUT
};

class CPeriphery
{
	bool bIsCreated;
	int MouseX, MouseY;
	bool InputStates[MAX_INPUT];
	BYTE KeyStates[256];
public:
	HWND game;

	void Initialise()
	{
		memset(InputStates, 0, sizeof(InputStates));
		bIsCreated = false;
		MouseX = 0;
		MouseY = 0;
	}

	void Create()
	{
		game = FindWindow(NULL, GAMEWINDOW);
		bIsCreated = true;
	}
	
	bool IsCreated()
	{
		return bIsCreated;
	}

	void UpdateMousePos()
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(game, &p);

		MouseX = p.x;
		MouseY = p.y;
	}

	void GetMousePos(int& x, int& y)
	{
		x = MouseX;
		y = MouseY;
	}

	void PollInputState(bool ShouldSave)
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			if (ShouldSave)
				InputStates[LBUTTON_DOWN] = true;
		}
		else
		{
			if (ShouldSave)
				InputStates[LBUTTON_DOWN] = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) & 1)
		{
			if (ShouldSave)
				InputStates[LBUTTON_CLICKED] = true;
		}
		else
		{
			if (ShouldSave)
				InputStates[LBUTTON_CLICKED] = false;
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			InputStates[KEY_INSERT_PRESSED] = true;
		}
		else
		{
			InputStates[KEY_INSERT_PRESSED] = false;
		}

		GetKeyboardState(KeyStates);
	}

	bool* GetInputStates()
	{
		return InputStates;
	}

	BYTE* GetKeyStates()
	{
		return KeyStates;
	}
};