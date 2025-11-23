#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <wrl/client.h>

#include "ConstantBuffer.h"

namespace DX = DirectX;

struct MaterialProperties
{
	int hasAmbientTexture;
	DX::XMFLOAT3 AmbientComponent;

	int hasDiffuseTexture;
	DX::XMFLOAT3 DiffuseComponent;

	int hasSpecularTexture;
	DX::XMFLOAT3 SpecularComponent;

	float SpecularExponent;
	float padding[3] = { 0.f,0.f,0.f }; // Padding to make size multiple of 16 bytes
};


class SubMesh
{
private:
	size_t m_startIndex = 0;
	size_t m_nrOfIndices = 0;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ambientTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuseTexture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularTexture = nullptr;

	ConstantBuffer m_materialBuffer;

public:
	SubMesh() = default;
	SubMesh(ID3D11Device* device, size_t startIndexValue, size_t nrOfIndicesInSubMesh, 
		ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV, 
		DX::XMFLOAT3 ambientComponent, DX::XMFLOAT3 diffuseComponent, DX::XMFLOAT3 specularComponent, float specularExponent);
	~SubMesh() = default;
	SubMesh(const SubMesh& other) = default;
	SubMesh& operator=(const SubMesh& other) = default;
	SubMesh(SubMesh&& other) = default;
	SubMesh& operator=(SubMesh&& other) = default;

	void Init(ID3D11Device* device, size_t startIndexValue, size_t nrOfIndicesInSubMesh,
		ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV,
		DX::XMFLOAT3 ambientComponent, DX::XMFLOAT3 diffuseComponent, DX::XMFLOAT3 specularComponent, float specularExponent);

	void PerformDrawCall(ID3D11DeviceContext* context) const;

	ID3D11ShaderResourceView* GetAmbientSRV() const;
	ID3D11ShaderResourceView* GetDiffuseSRV() const;
	ID3D11ShaderResourceView* GetSpecularSRV() const;
};