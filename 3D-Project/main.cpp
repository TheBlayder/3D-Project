#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <chrono>
#include <stdexcept>

#include "Window.h"
#include "Renderer.h"

#include "BaseScene.h"
#include "TestScene.h"

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
	
	Window* window = new Window(hInstance, nCmdShow, WIN_WIDTH, WIN_HEIGHT);

	Renderer renderer;
	if (!renderer.Init(window))
	{
		std::cout << "Failed to initialize renderer" << std::endl;
		return -1;
	}

	BaseScene* scene = new TestScene();
	bool hasParticles = true;
	scene->Init(renderer.GetDevice(), renderer.GetImmediateContext(), window, renderer.GetDCEMPixelShader(), renderer.GetReturnPixelShader(), hasParticles);


	bool wireframeMode = false;
	// === MAIN LOOP ===
	MSG msg = { };
	float deltaTime = 0.0f;
	using namespace std::chrono;
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		time_point<high_resolution_clock> start = high_resolution_clock::now();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Enable/disable wireframe mode to show tesselation effect
		if(window->GetInputHandler().isDown('T'))
		{
			if (wireframeMode)
			{
				wireframeMode = false;
				renderer.SetWireframe(false);
			}
			else
			{
				wireframeMode = true;
				renderer.SetWireframe(true);
			}
			window->GetInputHandler().setKeyState('T', InputHandler::RELEASED); // Prevent continuous toggling
		}

		renderer.RenderFrame(scene, deltaTime);

		time_point<high_resolution_clock> end = high_resolution_clock::now();
		duration<float> time = start - end;
		deltaTime = time.count();
	}

	delete scene;
	delete window;

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	return 0;
}