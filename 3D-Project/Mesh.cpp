#include "Mesh.h"

#include <stdexcept>
#include <vector>

#include "OBJ_Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh::Mesh(ID3D11Device* device, const std::string& filePath)
{
	Init(device, filePath);
}

void Mesh::Init(ID3D11Device* device, const std::string& filePath)
{
	objl::Loader loader;
	bool isLoaded = loader.LoadFile(filePath);
	if (!isLoaded)
	{
		throw std::runtime_error("Failed to load mesh from file: " + filePath);
	}

	size_t startIndex = 0;
	std::vector<unsigned int> tempIndices;
	std::vector<objl::Vertex> tempVertices;

	for(auto& mesh : loader.LoadedMeshes)
	{
		SubMesh subMesh;
		
	}


}

void Mesh::BindMeshBuffers(ID3D11DeviceContext* context) const
{
	ID3D11Buffer* vBuffer = m_vertexBuffer.GetBuffer();
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const
{
	m_subMeshes[subMeshIndex].PerformDrawCall(context);
}

size_t Mesh::GetNrOfSubMeshes() const
{
	return m_subMeshes.size();
}

ID3D11ShaderResourceView* Mesh::GetAmbientSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetAmbientSRV();
}

ID3D11ShaderResourceView* Mesh::GetDiffuseSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetDiffuseSRV();
}

ID3D11ShaderResourceView* Mesh::GetSpecularSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetSpecularSRV();
}
