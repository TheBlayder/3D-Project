#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Window.h"
#include "BaseScene.h"
#include "ConstantBuffer.h"
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

	// Views
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;

	// Input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;

	// Rasterizer states
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_defaultRasterizerState;

	// Sampler states
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_shadowSamplerState;

	// Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_DCEMPixelShader;

	Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader;

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

public:
	Renderer() = default;
	~Renderer() = default;

	bool Init(const Window& window);
	void RenderFrame(BaseScene* scene, const float deltaTime);

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetImmediateContext() { return m_immediateContext.Get(); }

	ID3D11PixelShader* GetDCEMPixelShader() { return m_DCEMPixelShader.Get(); }
	ID3D11PixelShader* GetReturnPixelShader() { return m_pixelShader.Get(); }
};