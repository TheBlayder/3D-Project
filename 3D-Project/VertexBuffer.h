#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_vertexBuffer;
	UINT m_nrOfVertices = 0;
	UINT m_vertexSize = 0;

public:
	VertexBuffer() = default;
	VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);
	~VertexBuffer() = default;

	void Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices);

	ID3D11Buffer* GetBuffer() const;
	ID3D11Buffer** GetBufferPtr();
	const UINT GetVertexSize() const;
	const UINT GetNrOfVertices() const;
};

