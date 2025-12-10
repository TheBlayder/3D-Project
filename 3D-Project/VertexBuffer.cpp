#include "VertexBuffer.h"

#include <iostream>

VertexBuffer::VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	Init(device, vertices, vertexSize, nrOfVertices);
}

void VertexBuffer::Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	m_vertexSize = vertexSize;
	m_nrOfVertices = nrOfVertices;

	D3D11_BUFFER_DESC vBufferDesc = {};
	vBufferDesc.ByteWidth = vertexSize * nrOfVertices;
	vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vBufferData = {};
	vBufferData.pSysMem = vertices;
	vBufferData.SysMemPitch = 0;
	vBufferData.SysMemSlicePitch = 0;
	
	HRESULT hr = device->CreateBuffer(&vBufferDesc, &vBufferData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Failed to create vertex buffer!" << std::endl;
	}
}

ID3D11Buffer* VertexBuffer::GetBuffer() const
{
	return m_vertexBuffer.Get();
}

ID3D11Buffer** VertexBuffer::GetBufferPtr()
{
	return m_vertexBuffer.GetAddressOf();
}

const UINT VertexBuffer::GetVertexSize() const
{
	return m_vertexSize;
}

const UINT VertexBuffer::GetNrOfVertices() const
{
	return m_nrOfVertices;
}