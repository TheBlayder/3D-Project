#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <chrono>

#include "EngineHandler.h"
#include "Window.h"
#include "Renderer.h"
#include "D3D11SetUp.h"
#include "PipelineSetUp.h"
#include "stb_image.h"

#define _CRTDBG_MAP_ALLOC

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Check for memory leaks


	// === WINDOW CREATION ===
	const UINT WIN_WIDTH = 800;
	const UINT WIN_HEIGHT = 600;
	
	EngineHandler engineHandler(hInstance, nCmdShow, WIN_HEIGHT, WIN_WIDTH);

	// === MAIN LOOP ===

	MSG msg = { };
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		using namespace std::chrono;
		time_point<high_resolution_clock> start = high_resolution_clock::now();

		TranslateMessage(&msg);
		DispatchMessage(&msg);



		time_point<high_resolution_clock> end = high_resolution_clock::now();
		duration<float> time = start - end;
		float deltaTime = time.count();
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	return 0;
}