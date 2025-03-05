#include "Window.h"

Window::Window(UINT width, UINT height) : m_width(width), m_height(height), m_hWindow(nullptr)
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

	this->m_hWindow = CreateWindowEx(0, CLASS_NAME, L"Project Engine", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, this->m_width, this->m_height, nullptr, nullptr, hInstance, this);

	if (this->m_hWindow == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(this->m_hWindow, nCmdShow);
	return true;
}


// === GET FUNCTIONS ===

HWND Window::GetWindowHandle() const
{
	return this->m_hWindow;
}

UINT Window::GetWidth() const
{
	return this->m_width;
}

UINT Window::GetHeight() const
{
	return this->m_height;
}
