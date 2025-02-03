#include <Windows.h>
#include <d3d11.h>
#include <iostream>

#include "Window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Check for memory leaks


	// === WINDOW CREATION ===
	const UINT WIN_WIDTH = 800;
	const UINT WIN_HEIGHT = 600;
	Window window(WIN_WIDTH, WIN_HEIGHT);
	if (!window.SetupWindow(hInstance, nCmdShow))
	{
		return -1;
	}


	// === MAIN LOOP === 
	MSG msg = { };
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}