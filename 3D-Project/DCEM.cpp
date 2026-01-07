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
		hr = device->CreateRenderTargetView(m_cubeMapTex.Get(), &rtvDesc, m_cubeMapRTVs[i].GetAddressOf());

		if (FAILED(hr))
			throw std::runtime_error("Could not create DCEM cube map RTV");
	}

	// SRV
	device->CreateShaderResourceView(m_cubeMapTex.Get(), nullptr, m_cubeMapSRV.GetAddressOf());

	// Depth texture
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = m_resolution;
	depthDesc.Height = m_resolution;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&depthDesc, nullptr, m_depthTex.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Could not create DCEM depth texture");

	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;

	for (UINT i = 0; i < 6; ++i)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateDepthStencilView(m_depthTex.Get(), &dsvDesc, m_DSVs[i].GetAddressOf());
		if (FAILED(hr))
			throw std::runtime_error("Could not create DCEM DSV");
	}



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

/// <summary>
/// Renders and draws one face of the DCEM at a time
/// </summary>
void DCEM::RenderAndDraw(ID3D11DeviceContext* context, const std::vector<BaseObject*>& sceneObjects, ConstantBuffer* worldBuffer, 
	ConstantBuffer* viewProjBuffer, Camera* camera, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, D3D11_VIEWPORT* returnVP)
{
	for (size_t i = 0; i < 6; ++i)
	{
		context->RSSetViewports(1, &m_viewport);
		Render(context, sceneObjects, worldBuffer, viewProjBuffer, i);

	}
	context->RSSetViewports(1, returnVP);
	context->PSSetShader(dcemPS, nullptr, 0);
	Draw(context, worldBuffer, viewProjBuffer, camera);

	// Reset bound SRV and constant buffer to avoid leaking state
	ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);

	ID3D11Buffer* nullCB[1] = { nullptr };
	context->PSSetConstantBuffers(1, 1, nullCB);

	context->PSSetShader(returnPS, nullptr, 0);
}

void DCEM::Render(ID3D11DeviceContext* context, const std::vector<BaseObject*>& sceneObjects, ConstantBuffer* worldBuffer, ConstantBuffer* viewProjBuffer, const size_t face)
{
	context->OMSetRenderTargets(1, m_cubeMapRTVs[face].GetAddressOf(), m_DSVs[face].Get());

	// Clear the face
	float clearColor[4] = { 0, 0, 0, 0 };
	context->ClearRenderTargetView(m_cubeMapRTVs[face].Get(), clearColor);
	context->ClearDepthStencilView(m_DSVs[face].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Update and set view-projection for this face in the vertex shader
	DX::XMFLOAT4X4 viewProjMatrix = m_cameras[face].GetViewProjMatrix();
	viewProjBuffer->Update(context, &viewProjMatrix);
	context->VSSetConstantBuffers(1, 1, viewProjBuffer->GetBufferPtr());

	// Draw all scene objects
	for (auto& obj : sceneObjects)
	{
		DX::XMFLOAT4X4 worldMatrix = obj->GetWorldMatrix();
		worldBuffer->Update(context, &worldMatrix);
		context->VSSetConstantBuffers(0, 1, worldBuffer->GetBufferPtr());

		obj->Draw(context);
	}

	// Unbind cubemap RTVs and restore previous OM targets and viewport
	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, nullptr);
}

void DCEM::Draw(ID3D11DeviceContext* context, ConstantBuffer* worldBuffer, ConstantBuffer* viewProjBuffer, Camera* camera)
{
	m_mesh.UnbindSubMeshResources(context);
	
	context->PSSetShaderResources(0, 1, m_cubeMapSRV.GetAddressOf());

	// Bind DCEMS world matrix
	DX::XMFLOAT4X4 worldMatrix = GetWorldMatrix();
	viewProjBuffer->Update(context, &worldMatrix);
	context->VSSetConstantBuffers(0, 1, worldBuffer->GetBufferPtr());

	// Reset camera view-proj matrix
	DirectX::XMFLOAT4X4 viewProjMatrix = camera->GetViewProjMatrix();
	viewProjBuffer->Update(context, &viewProjMatrix);
	context->VSSetConstantBuffers(1, 1, viewProjBuffer->GetBufferPtr());

	// Update camera position constant buffer for the pixel shader
	DX::XMFLOAT3 camPos = camera->GetPosition();
	DX::XMFLOAT4 camPos4 = { camPos.x, camPos.y, camPos.z, 1.0f };
	m_cameraBuffer.Update(context, &camPos4);
	context->PSSetConstantBuffers(0, 1, m_cameraBuffer.GetBufferPtr());

	m_mesh.BindMeshBuffers(context);
	for (size_t i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		m_mesh.PerformSubMeshDCEMDrawCall(context, i);
	}
}

const DirectX::XMFLOAT4X4 DCEM::GetWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	MatrixHelper::CreateWorldMatrix(worldMatrix, m_transform);
	return worldMatrix;
}