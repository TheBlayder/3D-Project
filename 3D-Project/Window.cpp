#include "Window.h"

Window::Window(UINT width, UINT height) : width(width), height(height), hWindow(nullptr)
{
	
}

Window::~Window()
{
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Window::SetupWindow(HINSTANCE hInstance, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	this->hWindow = CreateWindowEx(0, CLASS_NAME, L"Project Engine", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, this->width, this->height, nullptr, nullptr, hInstance, this);

	if (this->hWindow == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(this->hWindow, nCmdShow);
	return true;
}


// === GET FUNCTIONS ===

HWND Window::GetWindowHandle() const
{
	return this->hWindow;
}

UINT Window::GetWidth() const
{
	return this->width;
}

UINT Window::GetHeight() const
{
	return this->height;
}
