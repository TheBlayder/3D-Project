#include "DCEM.h"

#include <stdexcept>

#include "ReadCSO.h"
#include "HelperFuncs.h"

namespace DX = DirectX;

DCEM::DCEM(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS)
	: BaseObject(device, transform, folderPath, objectName)
{
	this->dcemPS = dcemPS;
	this->returnPS = returnPS;
	m_resolution = resolution;

	DX::XMFLOAT4 zero = { 0,0,0,0 };
	m_cameraBuffer = ConstantBuffer(device, sizeof(DX::XMFLOAT4), &zero);

	Init(device);
}

void DCEM::Init(ID3D11Device* device)
{
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
	texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_cubeMapTex.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Could not create DCEM cube map texture");

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.ArraySize = 1;

	for (UINT i = 0; i < 6; ++i)
	{
		uavDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateUnorderedAccessView(m_cubeMapTex.Get(), &uavDesc, m_cubeMapUAVs[i].GetAddressOf());

		if (FAILED(hr))
			throw std::runtime_error("Could not create DCEM cube map UAV #" + std::to_string(i));
	}

	// SRV
	hr = device->CreateShaderResourceView(m_cubeMapTex.Get(), nullptr, m_cubeMapSRV.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Could not create DCEM cube map SRV");

	// Camera setup
	ProjectionData projData = {};
	projData.fovInDeg = 90.f;
	projData.aspectRatio = 1.f;
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 100.f;
	float upRotations[6] = { DX::XM_PIDIV2, -DX::XM_PIDIV2,		0.0f,			0.0f,		0.0f, DX::XM_PI }; // Rotations around local up
	float rightRotations[6] = { 0.0f,		0.0f,			-DX::XM_PIDIV2, DX::XM_PIDIV2, 0.0f,  0.0f }; // Rotations around local right vector

	for (size_t i = 0; i < 6; ++i)
	{
		m_cameras[i].Init(device, projData, m_resolution, m_resolution, m_transform.GetPositionF3());

		DX::XMFLOAT3 upAxisF3(0.f, 1.f, 0.f);
		DX::XMVECTOR upAxis = DX::XMLoadFloat3(&upAxisF3);
		m_cameras[i].RotateAroundAxis(upRotations[i], upAxis);

		DX::XMFLOAT3 rightAxisF3(1.f, 0.f, 0.f);
		DX::XMVECTOR rightAxis = DX::XMLoadFloat3(&rightAxisF3);
		m_cameras[i].RotateAroundAxis(rightRotations[i], rightAxis);
	}
}

void DCEM::Draw(ID3D11DeviceContext* context) const
{
	context->PSSetShader(dcemPS, nullptr, 0);

	context->PSSetShaderResources(3, 1, m_cubeMapSRV.GetAddressOf());

	m_mesh.BindMeshBuffers(context);

	for (size_t i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		// Draw sub-meshes
		m_mesh.PerformSubMeshDrawCall(context, i);
	}

	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources(4, 1, &nullsrv);

	context->PSSetShader(this->returnPS, nullptr, 0);
}

const void DCEM::GetWorldMatrix(DX::XMFLOAT4X4& worldMatrix)
{
	MatrixHelper::CreateWorldMatrix(worldMatrix, m_transform);
}

std::array<ID3D11UnorderedAccessView**, 6> DCEM::GetUAVAdresses()
{
	std::array<ID3D11UnorderedAccessView**, 6> UAVs;
	for (int i = 0; i < 6; ++i)
	{
		UAVs[i] = m_cubeMapUAVs[i].GetAddressOf();
	}

	return UAVs;
}
