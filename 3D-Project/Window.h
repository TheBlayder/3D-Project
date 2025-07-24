#pragma once

#include <Windows.h>
#include <iostream>
#include <string>

class Window
{
private:

	HWND m_hWindow;
	UINT m_width, m_height;
	HINSTANCE m_hInstance;

public:

	Window(const HINSTANCE hInstance, int nCmdShow, UINT width = 0, UINT height = 0);
	~Window();

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetWindowHandle() const;
	UINT GetWidth() const;
	UINT GetHeight() const;
};


