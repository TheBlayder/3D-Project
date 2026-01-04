#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Window.h"
#include "BaseScene.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "DeferredHandler.h"

using namespace Microsoft;

class Renderer
{
private:
	// Direct3D components
	WRL::ComPtr<ID3D11Device> m_device;
	WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	WRL::ComPtr<IDXGISwapChain> m_swapChain;
	D3D11_VIEWPORT m_viewport;

	// Views
	WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;

	// Input layout
	WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Rasterizer states
	WRL::ComPtr<ID3D11RasterizerState> m_defaultRasterizerState;

	// Sampler states
	WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	WRL::ComPtr<ID3D11SamplerState> m_shadowSamplerState;

	// Shaders
	WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	WRL::ComPtr<ID3D11ComputeShader> m_computeShader;
	WRL::ComPtr<ID3D11PixelShader> m_DCEMPixelShader;

	// Constant buffers
	ConstantBuffer m_worldBuffer;
	ConstantBuffer m_viewProjectionBuffer;

	void CreateViewport(const Window& window);
	bool CreateDeviceAndSwapChain(const Window& window);
	bool CreateShaders(std::string& vShaderByteCodeOUT);
	bool CreateInputLayout(const std::string& vShaderByteCode);
	bool CreateUAV();
	bool CreateSamplerStates();
	bool CreateRasterizerStates();
	bool CreateConstantBuffers();

	void ShadowPass(BaseScene* scene);
	void GeometryPass(BaseScene* scene);
	void LightPass(BaseScene* scene);

	void RenderDCEMObjects(BaseScene* scene);

public:
	Renderer() = default;
	~Renderer() = default;

	bool Init(const Window& window);
	void RenderFrame(BaseScene* scene, const float deltaTime);

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetImmediateContext() { return m_immediateContext.Get(); }
};