#include "DCEM.h"

#include <stdexcept>

namespace DX = DirectX;

void DCEM::Update(float deltaTime)
{
}

void DCEM::Init(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = m_size;
	texDesc.Height = m_size;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_cubeMapTex.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Could not create DCEM cube map texture");

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for (size_t i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		hr = device->CreateRenderTargetView(m_cubeMapTex.Get(), &rtvDesc, m_cubeMapRTV.GetAddressOf());

		if (FAILED(hr))
			throw std::runtime_error("Could not create DCEM cube map RTV");
	}

	device->CreateShaderResourceView(m_cubeMapTex.Get(), nullptr, m_cubeMapSRV.GetAddressOf());

	// Camera setup
	ProjectionData projData = {};
	projData.fovInDeg = DX::XM_PIDIV2;
	projData.aspectRatio = 1.f;
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 100.f;
	float upRotations[6] = { DX::XM_PIDIV2, -DX::XM_PIDIV2, 0.0f, 0.0f, 0.0f, DX::XM_PI }; // Rotations around local up
	float rightRotations[6] = { 0.0f, 0.0f, -DX::XM_PIDIV2, DX::XM_PIDIV2, 0.0f, 0.0f }; // Rotations around local right vector

	for (size_t i = 0; i < 6; ++i)
	{
		m_cameras[i].Init(device, projData, m_transform.GetPositionF3());
		m_cameras[i].RotateAroundAxis(upRotations[i], DX::XMFLOAT3(0.f, 1.f, 0.f));
		m_cameras[i].RotateAroundAxis(rightRotations[i], DX::XMFLOAT3(1.f, 0.f, 0.f));
	}
}
