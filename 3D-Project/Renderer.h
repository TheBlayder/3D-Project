#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Window.h"
#include "BaseScene.h"
#include "ConstantBuffer.h"
#include "TestObject.h"
#include "GameObject.h"
#include "Camera.h"
#include "DeferredHandler.h"

class Renderer
{
private:
	// Direct3D components
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	D3D11_VIEWPORT m_viewport;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_defaultRasterizerState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

	// Constant buffers
	ConstantBuffer m_worldBuffer;
	ConstantBuffer m_viewProjectionBuffer;

	DeferredHandler* m_deferredHandler;
	Camera* m_camera;
	GameObject* m_test1;

	void CreateViewport(const Window& window);
	bool CreateDeviceAndSwapChain(const Window& window);
	bool CreateShaders(std::string& vShaderByteCodeOUT);
	bool CreateInputLayout(const std::string& vShaderByteCode);
	bool CreateUAV();
	bool CreateRenderTargetView();
	bool CreateDepthStencilView(const Window& window);
	bool CreateSamplerState();
	bool CreateRasterizerState();
	bool CreateConstantBuffers();

	void GeometryPass();
	void LightPass();

public:
	Renderer() = default;
	~Renderer();

	bool Init(const Window& window);
	//void RenderFrame(BaseScene* scene, float deltaTime);

	void RenderForward(); // Temporary function for testing
	void RenderDeferred(); // Temporary function for testing

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetImmediateContext();
};