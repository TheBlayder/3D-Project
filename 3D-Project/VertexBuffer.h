#pragma once

#include <d3d11.h>

class VertexBuffer
{
private:
	ID3D11Buffer* m_vertexBuffer;
	UINT nrOfVertices = 0;
	UINT vertexSize = 0;

public:
	VertexBuffer() = default;
	VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);
	~VertexBuffer() = default;

	void Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);

	ID3D11Buffer* GetBuffer() const;
	UINT GetVertexSize() const;
	UINT GetNrOfVertices() const;
};

