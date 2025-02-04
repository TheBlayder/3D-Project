#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

class Window
{
private:

	HWND hWindow;
	UINT width, height;

public:

	Window(UINT width, UINT height);
	~Window();

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool SetupWindow(HINSTANCE hInstance, int nCmdShow);

	HWND GetWindowHandle() const;
	UINT GetWidth() const;
	UINT GetHeight() const;
};


