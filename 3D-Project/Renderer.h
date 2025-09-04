#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "Window.h"
#include "ConstantBuffer.h"

class Renderer
{
private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_immediateContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;

	ID3D11RasterizerState* m_defaultRasterizerState = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ConstantBuffer worldBuffer;
	ConstantBuffer viewProjectionBuffer;


	void CreateViewport(const Window& window);
	bool CreateDeviceAndSwapChain(const Window& window);
	bool CreateInputLayout();

public:
	Renderer();
	~Renderer();

	bool Init(const Window& window);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetImmediateContext();


};