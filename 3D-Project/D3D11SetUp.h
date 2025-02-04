#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "Window.h"

class D3D11SetUp
{

public:
	D3D11SetUp();
	~D3D11SetUp();

	bool SetUp(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain,
		ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, Window*& window);

private:
	bool CreateSwapChain(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, Window* window);
	bool CreateRenderTargetView(ID3D11Device*& device, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv);
	bool CreateDepthStencilView(ID3D11Device*& device, Window* window, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView);
	void SetViewPort(D3D11_VIEWPORT& viewport, Window*& window);

};