#include "VertexBuffer.h"

#include <iostream>

VertexBuffer::VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	Init(device, vertices, vertexSize, nrOfVertices);
}

void VertexBuffer::Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	this->vertexSize = vertexSize;
	this->nrOfVertices = nrOfVertices;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = vertexSize * nrOfVertices;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	
	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create vertex buffer!" << std::endl;
	}
}

ID3D11Buffer* VertexBuffer::GetBuffer() const
{
	return m_vertexBuffer;
}

UINT VertexBuffer::GetVertexSize() const
{
	return vertexSize;
}

UINT VertexBuffer::GetNrOfVertices() const
{
	return nrOfVertices;
}
