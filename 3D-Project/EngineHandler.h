#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <vector>

#include "Window.h"
#include "D3D11SetUp.h"
#include "PipelineSetUp.h"
#include "Renderer.h"
#include "GameObject.h"
#include "ObjectParser.h"
#include "Camera.h"


class EngineHandler
{
private:
	Window* m_window;
	PipelineSetUp* m_pipelineSetUp;
	D3D11SetUp* m_d3d11SetUp;
	Renderer* m_renderer;

	Camera* m_camera;
	std::vector<GameObject*> m_gameObjects;


	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_immediateContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11Texture2D* m_dsTexture = nullptr;
	ID3D11DepthStencilView* m_dsView = nullptr;
	D3D11_VIEWPORT m_viewport;

	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader* m_pShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_SRVTexture;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_VScBuffer;
	ID3D11Buffer* m_PScBuffer;
	unsigned char* m_textureData = nullptr;

public:
	EngineHandler(HINSTANCE hInstance, int nCmdShow, const UINT height, const UINT width);
	~EngineHandler();

	void RenderFrame();
	bool AddObject(std::string filePath, DX::XMFLOAT3 scale, DX::XMFLOAT3 rotation, DX::XMFLOAT3 position, bool isStatic);

	Window* GetWindow() const;
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetImmediateContext() const;

private:
	bool CreateCamera();
};

