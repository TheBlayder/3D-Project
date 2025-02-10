#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <chrono>

#include "Window.h"
#include "Renderer.h"
#include "D3D11SetUp.h"
#include "PipelineSetUp.h"
#include "stb_image.h"

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
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* SRVTexture;
	ID3D11SamplerState* samplerState;
	ID3D11Buffer* VScBuffer;
	ID3D11Buffer* PScBuffer;

	unsigned char* textureData = nullptr;

	PipelineSetUp* pipelineSetUp = new PipelineSetUp();
	if (!pipelineSetUp->SetUp(device, immediateContext, vertexBuffer, vShader, pShader, inputLayout, VScBuffer, PScBuffer, texture, SRVTexture, samplerState, textureData))
	{
		std::cerr << "Error setting up pipeline!" << std::endl;
		return -1;
	}

	// === MAIN LOOP ===

	UINT stride, offset, nrOfVertices;
	Renderer* renderer = new Renderer();

	MSG msg = { };
	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		using namespace std::chrono;
		time_point<high_resolution_clock> start = high_resolution_clock::now();

		TranslateMessage(&msg);
		DispatchMessage(&msg);


		renderer->RenderFrame(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, SRVTexture, samplerState, stride, offset, nrOfVertices);



		time_point<high_resolution_clock> end = high_resolution_clock::now();
		duration<float> time = start - end;
		float deltaTime = time.count();
	}

	// === CLEANUP ===
	//
	// 
	stbi_image_free(textureData);
	// Release D3D11 objects
	PScBuffer->Release();
	VScBuffer->Release();
	samplerState->Release();
	SRVTexture->Release();
	texture->Release();
	vertexBuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	dsView->Release();
	dsTexture->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	// Release heap objects
	delete renderer;
	delete pipelineSetUp;
	delete d3d11SetUp;
	delete window;

	return 0;
}