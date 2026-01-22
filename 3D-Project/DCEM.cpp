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

	for (size_t i = 0; i < m_cameras.size(); ++i)
	{
		m_cameras[i] = new Camera();
	}

	Init(device);
}

DCEM::~DCEM()
{
	for (size_t i = 0; i < m_cameras.size(); ++i)
	{
		delete m_cameras[i];
	}
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

	// Standard DirectX cube map orientation (Right-handed to Left-handed conversion)
	float rotations[6][3] = {
		{ 0.0f,   90.0f,  0.0f },   // +X: Right
		{ 0.0f,  -90.0f,  0.0f },   // -X: Left
		{ -90.0f,  180.0f,  0.0f }, // +Y: Top
		{ 90.0f,  0.0f,  0.0f },    // -Y: Bottom
		{ 0.0f,    0.0f,  0.0001f },// +Z: Front
		{ 0.0f,  180.0f,  0.0f }    // -Z: Back
	};

	for (size_t i = 0; i < 6; ++i)
	{
		m_cameras[i]->Init(device, projData, m_resolution, m_resolution, m_transform.GetPositionF3());

		DX::XMVECTOR rotation = DX::XMVectorSet(
			rotations[i][0],
			rotations[i][1],
			rotations[i][2],
			0.0f
		);
		m_cameras[i]->GetTransform().SetRotation(rotation);
	}
}

void DCEM::Draw(ID3D11DeviceContext* context) const
{
	context->PSSetShader(dcemPS, nullptr, 0);

	context->PSSetShaderResources(3, 1, m_cubeMapSRV.GetAddressOf());

	context->PSSetConstantBuffers(6, 1, m_cameraBuffer.GetBufferPtr());

	m_mesh.BindMeshBuffers(context);

	for (size_t i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		// Draw sub-meshes
		m_mesh.PerformSubMeshDrawCall(context, i);
	}

	ID3D11ShaderResourceView* nullsrv = nullptr;
	context->PSSetShaderResources(3, 1, &nullsrv);

	ID3D11Buffer* nullBuffer = nullptr;
	context->PSSetConstantBuffers(6, 1, &nullBuffer);

	context->PSSetShader(this->returnPS, nullptr, 0);
}

void DCEM::Update(ID3D11DeviceContext* context, float deltaTime, Camera* cam)
{
	using namespace DirectX;
	XMVECTOR camPos = cam->GetPosition();
	XMFLOAT4 camPosF4;
	XMStoreFloat4(&camPosF4, camPos);

	m_cameraBuffer.Update(context, &camPosF4);
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