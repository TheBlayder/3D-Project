#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Window.h"
#include "BaseScene.h"
#include "ConstantBuffer.h"

class Renderer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_defaultRasterizerState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

	ConstantBuffer worldBuffer;
	ConstantBuffer viewProjectionBuffer;

	void CreateViewport(const Window& window);
	bool CreateDeviceAndSwapChain(const Window& window);
	bool CreateShaders(std::string& vShaderByteCodeOUT);
	bool CreateInputLayout(const std::string& vShaderByteCode);
	bool CreateUAV();
	bool CreateSamplerState();
	bool CreateRasterizerState();

public:
	Renderer();
	~Renderer() = default;

	bool Init(const Window& window);
	//void Render(BaseScene* scene); // Argument okända för tillfället

	void RenderFrame(); // Temporary function for testing

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetImmediateContext();
};