#pragma once

#define DIRECTXTK_WIC_LOADER_FLAGS

#include <d3d11.h>
#include <vector>
#include <string>

#include "SubMesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
private:
	std::vector<SubMesh> m_subMeshes;
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	void CreateDefaultTexture(ID3D11Device* device, ID3D11ShaderResourceView** textureSRV);

public:
	Mesh() = default;
	Mesh(ID3D11Device* device, const std::string& folderPath, const std::string& objectName);
	~Mesh() = default;
	Mesh(const Mesh & other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	void Init(ID3D11Device* device, const std::string& folderPath, const std::string& objectName);

	void BindMeshBuffers(ID3D11DeviceContext* context) const;
	void PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const;

	size_t GetNrOfSubMeshes() const;
	ID3D11ShaderResourceView* GetAmbientSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetDiffuseSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetSpecularSRV(size_t subMeshIndex) const;
};