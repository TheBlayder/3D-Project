#pragma once

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

public:
	Mesh() = default;
	Mesh(ID3D11Device* device, const std::string& filePath);
	~Mesh() = default;
	Mesh(const Mesh & other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	void Init(ID3D11Device* device, const std::string& filePath);

	void BindMeshBuffers(ID3D11DeviceContext* context) const;
	void PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const;

	size_t GetNrOfSubMeshes() const;
	ID3D11ShaderResourceView* GetAmbientSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetDiffuseSRV(size_t subMeshIndex) const;
	ID3D11ShaderResourceView* GetSpecularSRV(size_t subMeshIndex) const;
};