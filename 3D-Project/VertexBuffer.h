#pragma once

#include <d3d11.h>

class VertexBuffer
{
private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer** m_vertexBufferPtr = &m_vertexBuffer;
	UINT m_nrOfVertices = 0;
	UINT m_vertexSize = 0;

public:
	VertexBuffer() = default;
	VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);
	~VertexBuffer();

	void Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);

	ID3D11Buffer* GetBuffer() const;
	ID3D11Buffer** GetBufferPtr() const;
	UINT GetVertexSize() const;
	UINT GetNrOfVertices() const;
};

