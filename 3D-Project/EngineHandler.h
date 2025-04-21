#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <vector>

#include "Window.h"
#include "D3D11SetUp.h"
#include "PipelineSetUp.h"
#include "Renderer.h"
#include "ObjectParser.h"
#include "BaseGameObject.h"
#include "Camera.h"


class EngineHandler
{
private:
	Window* m_window = nullptr;
	PipelineSetUp* m_pipelineSetUp = nullptr;
	D3D11SetUp* m_d3d11SetUp = nullptr;
	Renderer* m_renderer = nullptr;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_immediateContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11Texture2D* m_dsTexture = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	D3D11_VIEWPORT m_viewport;

	ID3D11VertexShader* m_vShader = nullptr;
	ID3D11PixelShader* m_pShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_SRVTexture = nullptr;
	ID3D11SamplerState* m_samplerState = nullptr;
	ID3D11Buffer* m_VScBuffer = nullptr;
	ID3D11Buffer* m_PScBuffer = nullptr;
	unsigned char* m_textureData = nullptr;

public:
	EngineHandler(HINSTANCE hInstance, int nCmdShow, const UINT height, const UINT width);
	~EngineHandler();

	Window* GetWindow() const;
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetImmediateContext() const;

private:
	
};

