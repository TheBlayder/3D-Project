#include "VertexBuffer.h"

#include <iostream>

VertexBuffer::VertexBuffer(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	Init(device, vertices, vertexSize, nrOfVertices);
}

VertexBuffer::~VertexBuffer()
{
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
}

void VertexBuffer::Init(ID3D11Device*& device, const void* vertices, const UINT& vertexSize, const UINT& nrOfVertices)
{
	this->m_vertexSize = vertexSize;
	this->m_nrOfVertices = nrOfVertices;

	D3D11_BUFFER_DESC vBufferDesc;
	vBufferDesc.ByteWidth = vertexSize * nrOfVertices;
	vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vBufferData;
	vBufferData.pSysMem = vertices;
	vBufferData.SysMemPitch = 0;
	vBufferData.SysMemSlicePitch = 0;
	
	HRESULT hr = device->CreateBuffer(&vBufferDesc, &vBufferData, &m_vertexBuffer);
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
	return m_vertexSize;
}

UINT VertexBuffer::GetNrOfVertices() const
{
	return m_nrOfVertices;
}

void VertexBuffer::Update(ID3D11DeviceContext* context, const void* vertices, const UINT& size)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices, size);
	context->Unmap(m_vertexBuffer, 0);
}
