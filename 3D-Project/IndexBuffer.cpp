#include "IndexBuffer.h"
#include <stdexcept>

IndexBuffer::IndexBuffer(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData)
{
	Init(device, nrOfIndicesInBuffer, indexData);
}

IndexBuffer::~IndexBuffer()
{
	if (m_buffer)
	{
		m_buffer->Release();
	}
}

void IndexBuffer::Init(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData)
{
	m_nrOfIndices = nrOfIndicesInBuffer;
	
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(indexData) * nrOfIndicesInBuffer);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA bufferData;
	bufferData.pSysMem = indexData;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;

	
	HRESULT hr = device->CreateBuffer(&bufferDesc, &bufferData, &m_buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create index buffer");
	}
}

size_t IndexBuffer::GetNrOfIndices() const
{
	return m_nrOfIndices;
}

ID3D11Buffer* IndexBuffer::GetBuffer() const
{
	return m_buffer;
}