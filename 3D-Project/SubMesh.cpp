#include "SubMesh.h"

#include <iostream>

SubMesh::SubMesh(ID3D11Device* device, size_t startIndexValue, size_t nrOfIndicesInSubMesh,
	ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV,
	DX::XMFLOAT3 ambientComponent, DX::XMFLOAT3 diffuseComponent, DX::XMFLOAT3 specularComponent, float specularExponent)
{
	Init(device, startIndexValue, nrOfIndicesInSubMesh, 
		ambientTextureSRV, diffuseTextureSRV, specularTextureSRV, 
		ambientComponent, diffuseComponent, specularComponent, specularExponent);
}

void SubMesh::Init(ID3D11Device* device, size_t startIndexValue, size_t nrOfIndicesInSubMesh,
	ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV,
	DX::XMFLOAT3 ambientComponent, DX::XMFLOAT3 diffuseComponent, DX::XMFLOAT3 specularComponent, float specularExponent)
{
	m_startIndex = startIndexValue;
	m_nrOfIndices = nrOfIndicesInSubMesh;
	m_ambientTexture = ambientTextureSRV;
	m_diffuseTexture = diffuseTextureSRV;
	m_specularTexture = specularTextureSRV;

	MaterialProperties materialProps = {
		(bool)ambientTextureSRV, ambientComponent,
		(bool)diffuseTextureSRV, diffuseComponent,
		(bool)specularTextureSRV, specularComponent,
		specularExponent
	};

	m_materialBuffer.Init(device, sizeof(MaterialProperties), &materialProps);
}

void SubMesh::PerformDrawCall(ID3D11DeviceContext* context) const
{
	ID3D11ShaderResourceView* SRVs[3] = {
		m_ambientTexture.Get(),
		m_diffuseTexture.Get(),
		m_specularTexture.Get()
	};
	context->PSSetShaderResources(0, 3, SRVs);

	ID3D11Buffer* cb = m_materialBuffer.GetBuffer();
	context->PSSetConstantBuffers(0, 1,&cb);

	context->DrawIndexed(static_cast<UINT>(m_nrOfIndices), static_cast<UINT>(m_startIndex), 0);
}

ID3D11ShaderResourceView* SubMesh::GetAmbientSRV() const
{
	return m_ambientTexture.Get();
}

ID3D11ShaderResourceView* SubMesh::GetDiffuseSRV() const
{
	return m_diffuseTexture.Get();
}

ID3D11ShaderResourceView* SubMesh::GetSpecularSRV() const
{
	return m_specularTexture.Get();
}
