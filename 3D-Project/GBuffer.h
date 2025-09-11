#pragma once
#include "d3d11.h"
#include <stdexcept>


class GBuffer
{
private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;

public:
	GBuffer() = default;
	inline GBuffer(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT);
	inline ~GBuffer() = default;

	inline void Init(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT);

	inline ID3D11Texture2D* GetTexture();
	inline ID3D11ShaderResourceView* GetSRV();
	inline ID3D11RenderTargetView* GetRTV();
};


inline GBuffer::GBuffer(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT)
{
	Init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
}

inline void GBuffer::Init(ID3D11Device* device, const UINT WINDOW_WIDTH, const UINT WINDOW_HEIGHT)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = WINDOW_WIDTH;
	textureDesc.Height = WINDOW_HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create GBuffer texture.");
	}

	hr = device->CreateShaderResourceView(m_texture, nullptr, &m_srv);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create GBuffer SRV.");
	}

	hr = device->CreateRenderTargetView(m_texture, nullptr, &m_rtv);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create GBuffer RTV.");
	}
}

inline ID3D11Texture2D* GBuffer::GetTexture()
{
	return nullptr;
}

inline ID3D11ShaderResourceView* GBuffer::GetSRV()
{
	return m_srv;
}

inline ID3D11RenderTargetView* GBuffer::GetRTV()
{
	return m_rtv;
}