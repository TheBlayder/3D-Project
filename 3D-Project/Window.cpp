#include "Window.h"

#include <exception>

Window::Window(HINSTANCE instance, int nCmdShow, UINT width, UINT height) : m_hInstance(instance), m_width(width), m_height(height), m_hWindow(nullptr)
{
	// Register the window class
	const wchar_t CLASS_NAME[] = L"WindowClass";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = StaticWindowProc;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = CLASS_NAME;

	if(!RegisterClassEx(&wc));
		throw std::runtime_error("Failed to register window class: " + std::to_string(GetLastError()));

	// If width and height are not specified, use the primary monitor's dimensions
	if(m_width == 0 && m_height == 0)
	{
		MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
		if (GetMonitorInfo(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
		{
			m_width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			m_height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		}
	}

	// Create the window
	m_hWindow = CreateWindowEx(0, CLASS_NAME, L"Project Engine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
							   0, m_width, m_height, nullptr, nullptr, m_hInstance, this);

	if (m_hWindow == nullptr)
		throw std::runtime_error("Failed to create window: " + std::to_string(GetLastError()));

	ShowWindow(m_hWindow, nCmdShow);
}

Window::~Window()
{
	if (m_hWindow) { DestroyWindow(m_hWindow); }
	UnregisterClass(L"WindowClass", m_hInstance);
}


LRESULT Window::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_NCCREATE)
	{
		// Retrieve the 'this' pointer from the CREATESTRUCT
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		Window* window = static_cast<Window*>(create->lpCreateParams);

		// Set the 'this' pointer in the window's user data
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->m_hWindow = hWnd;
		return window->WindowProc(hWnd, message, wParam, lParam);
	}
	else
	{
		// Retrieve the 'this' pointer from the window's user data
		Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (window != nullptr)
		{
			return window->WindowProc(hWnd, message, wParam, lParam);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// === HANDLER FOR WINDOW MESSAGES ===
LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		// Keyboard input handling
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			return 0;

		// Mouse input handling
		case WM_MOUSEMOVE:
			// Handle mouse movement
			return 0;
		case WM_LBUTTONDOWN:
			// Handle left mouse button down
			return 0;
		case WM_LBUTTONUP:
			// Handle left mouse button up
			return 0;
		case WM_RBUTTONDOWN:
			// Handle right mouse button down
			return 0;
		case WM_RBUTTONUP:
			// Handle right mouse button up
			return 0;

		// Destruction of the window
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

}

// === GETTERS ===

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
