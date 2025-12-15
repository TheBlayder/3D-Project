#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <array>
#include <iostream>

#include "GBuffer.h"

/// <summary>
/// Class handling deferred rendering resources like G-Buffers and depth stencil as well as binding them.
/// </summary>
class DeferredHandler
{
private: 	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

	GBuffer m_positionGBuffer;
	GBuffer m_normalGBuffer;
	GBuffer m_diffuseGBuffer;
	GBuffer m_ambientGBuffer;
	GBuffer m_specularGBuffer;

public:
	DeferredHandler() = default;
	DeferredHandler(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT);
	~DeferredHandler() = default;

	inline bool Init(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT);
	inline void BindGeometryPass(ID3D11DeviceContext* context);
	inline void BindLightPass(ID3D11DeviceContext* context);
	inline void ClearBuffers(ID3D11DeviceContext* context, std::array<float, 4> clearColor);

	ID3D11DepthStencilView* GetDSV() { return m_DSV.Get(); }
};

inline DeferredHandler::DeferredHandler(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT)
{
	Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
}

inline bool DeferredHandler::Init(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT)
{
	// Init G-Buffers
	m_positionGBuffer.Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_normalGBuffer.Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_diffuseGBuffer.Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_ambientGBuffer.Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
	m_specularGBuffer.Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	// Depth stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = WINDOW_WIDTH;
	depthStencilDesc.Height = WINDOW_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	
	HRESULT hr = device->CreateTexture2D(&depthStencilDesc, nullptr, m_depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Error creating DeferredHandler depth stencil buffer!" << std::endl;
		return false;
	}

	hr = device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_DSV.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Error creating DeferredHandler depth stencil view!" << std::endl;
		return false;
	}

	return true;
}

inline void DeferredHandler::BindGeometryPass(ID3D11DeviceContext* context)
{
	// Unbind from compute shader
	ID3D11ShaderResourceView* nullSRVs[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	context->CSSetShaderResources(0,5, nullSRVs);

	// Bind G-Buffers for geometry pass write
	ID3D11RenderTargetView* RTVs[] = {
		m_positionGBuffer.GetRTV(),
		m_normalGBuffer.GetRTV(),
		m_diffuseGBuffer.GetRTV(),
		m_ambientGBuffer.GetRTV(),
		m_specularGBuffer.GetRTV()
	};
	context->OMSetRenderTargets(5, RTVs, m_DSV.Get());
}

inline void DeferredHandler::BindLightPass(ID3D11DeviceContext* context)
{
	// Unbind render targets from geometry pass write
	ID3D11RenderTargetView* nullRTVs[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	context->OMSetRenderTargets(5, nullRTVs, nullptr);
	
	// Bind G-Buffers as shader resource views for light pass read in compute shader
	ID3D11ShaderResourceView* SRVs[] = {
		m_positionGBuffer.GetSRV(),
		m_normalGBuffer.GetSRV(),
		m_diffuseGBuffer.GetSRV(),
		m_ambientGBuffer.GetSRV(),
		m_specularGBuffer.GetSRV()
	};
	context->CSSetShaderResources(0, 5, SRVs);
}

inline void DeferredHandler::ClearBuffers(ID3D11DeviceContext* context, std::array<float, 4> clearColor = {0.f, 0.f, 0.f, 0.f})
{
	float zeroClear[4] = { 0.f, 0.f, 0.f, 0.f };
	context->ClearRenderTargetView(m_positionGBuffer.GetRTV(), zeroClear);
	context->ClearRenderTargetView(m_normalGBuffer.GetRTV(), zeroClear);
	context->ClearRenderTargetView(m_diffuseGBuffer.GetRTV(), clearColor.data());
	context->ClearRenderTargetView(m_ambientGBuffer.GetRTV(), clearColor.data());
	context->ClearRenderTargetView(m_specularGBuffer.GetRTV(), clearColor.data());
	
	context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
