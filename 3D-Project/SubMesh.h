#pragma once

#include <d3d11.h>
#include <string>

class SubMesh
{
private:
	size_t m_startIndex = 0;
	size_t m_nrOfIndices = 0;

	ID3D11ShaderResourceView* m_ambientTexture = nullptr;
	ID3D11ShaderResourceView* m_diffuseTexture = nullptr;
	ID3D11ShaderResourceView* m_specularTexture = nullptr;

public:
	SubMesh() = default;
	SubMesh(size_t startIndexValue, size_t nrOfIndicesInSubMesh, ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV);
	~SubMesh() = default;
	SubMesh(const SubMesh& other) = default;
	SubMesh& operator=(const SubMesh& other) = default;
	SubMesh(SubMesh&& other) = default;
	SubMesh& operator=(SubMesh&& other) = default;

	void Init(size_t startIndexValue, size_t nrOfIndicesInSubMesh, ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV);

	void PerformDrawCall(ID3D11DeviceContext* context) const;

	ID3D11ShaderResourceView* GetAmbientSRV() const;
	ID3D11ShaderResourceView* GetDiffuseSRV() const;
	ID3D11ShaderResourceView* GetSpecularSRV() const;
};