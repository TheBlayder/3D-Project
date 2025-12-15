#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <chrono>

#include "Window.h"
#include "Renderer.h"

#define _CRTDBG_MAP_ALLOC

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Check for memory leaks

	const UINT WIN_WIDTH = 1280;
	const UINT WIN_HEIGHT = 720;
	
	Window window(hInstance, nCmdShow, WIN_WIDTH, WIN_HEIGHT);

	Renderer renderer;
	if (!renderer.Init(window))
	{
		std::cout << "Failed to initialize renderer" << std::endl;
		return -1;
	}

	// === MAIN LOOP ===

	MSG msg = { };
	using namespace std::chrono;
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		time_point<high_resolution_clock> start = high_resolution_clock::now();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		renderer.RenderDeferred(); // Temporary function for testing

		time_point<high_resolution_clock> end = high_resolution_clock::now();
		duration<float> time = start - end;
		float deltaTime = time.count();
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	return 0;
}