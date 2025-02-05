#include <Windows.h>
#include <d3d11.h>
#include <iostream>

#include "Window.h"
#include "Renderer.h"
#include "D3D11SetUp.h"
#include "PipelineSetUp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // Check for memory leaks


	// === WINDOW CREATION ===
	const UINT WIN_WIDTH = 800;
	const UINT WIN_HEIGHT = 600;
	Window* window = new Window(WIN_WIDTH, WIN_HEIGHT);
	if (!window->SetupWindow(hInstance, nCmdShow))
	{
		return -1;
	}

	// === D3D11 SETUP ===
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11Texture2D* dsTexture = nullptr;
	ID3D11DepthStencilView* dsView = nullptr;
	D3D11_VIEWPORT viewport;

	D3D11SetUp* d3d11SetUp = new D3D11SetUp;
	if (!d3d11SetUp->SetUp(device, immediateContext, swapChain, rtv, dsTexture, dsView, viewport, window))
	{
		std::cerr << "Error setting up D3D11!" << std::endl;
		return -1;
	}

	// === PIPELINE SETUP ===
	PipelineSetUp* pipelineSetUp = new PipelineSetUp();
	

	// === MAIN LOOP ===
	Renderer* renderer = new Renderer();

	MSG msg = { };
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	// === CLEANUP ===
	//
	// Release D3D11 objects
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	// Release heap objects
	delete pipelineSetUp;
	delete d3d11SetUp;
	delete renderer;
	delete window;

	return 0;
}