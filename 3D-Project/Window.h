#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

class Window
{
private:

	UINT width, height;

public:
	HWND hWindow;

	Window(UINT width, UINT height);
	~Window();

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool SetupWindow(HINSTANCE hInstance, int nCmdShow);

	UINT GetWidth();
	UINT GetHeight();
};


