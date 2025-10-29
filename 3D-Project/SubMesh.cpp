#include "SubMesh.h"

SubMesh::SubMesh(size_t startIndexValue, size_t nrOfIndicesInSubMesh, ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV)
{
	Init(startIndexValue, nrOfIndicesInSubMesh, ambientTextureSRV, diffuseTextureSRV, specularTextureSRV);
}

void SubMesh::Init(size_t startIndexValue, size_t nrOfIndicesInSubMesh, ID3D11ShaderResourceView* ambientTextureSRV, ID3D11ShaderResourceView* diffuseTextureSRV, ID3D11ShaderResourceView* specularTextureSRV)
{
	m_startIndex = startIndexValue;
	m_nrOfIndices = nrOfIndicesInSubMesh;
	m_ambientTexture = ambientTextureSRV;
	m_diffuseTexture = diffuseTextureSRV;
	m_specularTexture = specularTextureSRV;
}

void SubMesh::PerformDrawCall(ID3D11DeviceContext* context) const
{
	//context->PSSetConstantBuffers(0, 0, nullptr);

	ID3D11ShaderResourceView* SRVs[3] = { m_ambientTexture, m_diffuseTexture, m_specularTexture };
	context->PSSetShaderResources(0, 3, SRVs);

	context->DrawIndexed(static_cast<UINT>(m_nrOfIndices), static_cast<UINT>(m_startIndex), 0);
}

ID3D11ShaderResourceView* SubMesh::GetAmbientSRV() const
{
	return m_ambientTexture;
}

ID3D11ShaderResourceView* SubMesh::GetDiffuseSRV() const
{
	return m_diffuseTexture;
}

ID3D11ShaderResourceView* SubMesh::GetSpecularSRV() const
{
	return m_specularTexture;
}
