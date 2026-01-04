#include "DCEM.h"

#include <stdexcept>

#include "ReadCSO.h"
#include "HelperFuncs.h"

namespace DX = DirectX;

DCEM::DCEM(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName)
{
	m_transform = transform;
	m_resolution = resolution;
	DX::XMFLOAT4 zero = { 0,0,0,0 };
	m_cameraBuffer = ConstantBuffer(device, sizeof(DX::XMFLOAT4), &zero);
	m_mesh.Init(device, folderPath, objectName);
	Init(device);
}

void DCEM::Init(ID3D11Device* device)
{
	// Viewport
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(m_resolution);
	m_viewport.Height = static_cast<FLOAT>(m_resolution);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Cube map texture
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = m_resolution;
	texDesc.Height = m_resolution;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_cubeMapTex.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Could not create DCEM cube map texture");

	// RTVs
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 1;

	for (UINT i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateRenderTargetView(m_cubeMapTex.Get(), &rtvDesc, m_cubeMapRTV.GetAddressOf());

		if (FAILED(hr))
			throw std::runtime_error("Could not create DCEM cube map RTV");
	}

	// SRV
	device->CreateShaderResourceView(m_cubeMapTex.Get(), nullptr, m_cubeMapSRV.GetAddressOf());

	// Camera setup
	ProjectionData projData = {};
	projData.fovInDeg = 90.f;
	projData.aspectRatio = 1.f;
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 100.f;
	float upRotations[6] = { DX::XM_PIDIV2, -DX::XM_PIDIV2, 0.0f, 0.0f, 0.0f, DX::XM_PI }; // Rotations around local up
	float rightRotations[6] = { 0.0f, 0.0f, -DX::XM_PIDIV2, DX::XM_PIDIV2, 0.0f, 0.0f }; // Rotations around local right vector

	for (size_t i = 0; i < 6; ++i)
	{
		m_cameras[i].Init(device, projData, m_resolution, m_resolution, m_transform.GetPositionF3());
		m_cameras[i].RotateAroundAxis(upRotations[i], DX::XMFLOAT3(0.f, 1.f, 0.f));
		m_cameras[i].RotateAroundAxis(rightRotations[i], DX::XMFLOAT3(1.f, 0.f, 0.f));
	}
}

void DCEM::Draw(ID3D11DeviceContext* context, const DX::XMFLOAT3& cameraPosition, ID3D11PixelShader* dcemPS)
{
	// Set up states
	context->PSSetShader(dcemPS, nullptr, 0);
	context->RSSetViewports(1, &m_viewport);

	context->PSSetShaderResources(0, 1, m_cubeMapSRV.GetAddressOf());

	// Update camera position buffer
	DX::XMFLOAT4 camPos = DX::XMFLOAT4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.f);
	m_cameraBuffer.Update(context, &camPos);
	context->PSSetConstantBuffers(1, 1, m_cameraBuffer.GetBufferPtr());

	m_mesh.BindMeshBuffers(context);

	for (UINT i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		m_mesh.PerformSubMeshDrawCall(context, i);
	}

	// Reset states
	ID3D11Buffer* nullCB[1] = { nullptr };
	context->PSSetConstantBuffers(1, 1, nullCB);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	context->PSSetShaderResources(0, 1, nullSRV);
}

const DirectX::XMFLOAT4X4 DCEM::GetWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	MatrixHelper::CreateWorldMatrix(worldMatrix, m_transform);
	return worldMatrix;
}