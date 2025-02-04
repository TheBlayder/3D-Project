#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class D3D11SetUp
{

public:
	D3D11SetUp();
	~D3D11SetUp();

private:
	ID3D11Device* device;
	ID3D11DeviceContext* immediateContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* dsView;
	D3D11_VIEWPORT viewport;

public:

};