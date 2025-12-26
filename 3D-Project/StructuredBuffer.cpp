#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer, void* bufferData, bool dynamic)
{
	Init(device, sizeOfElement, nrOfElementsInBuffer, bufferData, dynamic);
}

void StructuredBuffer::Init(ID3D11Device* device, UINT sizeOfElement, size_t nrOfElementsInBuffer, void* bufferData, bool dynamic)
{
	m_elementSize = sizeOfElement;
	m_nrOfElements = nrOfElementsInBuffer;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeOfElement * nrOfElementsInBuffer);
	bufferDesc.StructureByteStride = sizeOfElement;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = bufferData;

	HRESULT hr = device->CreateBuffer(&bufferDesc, bufferData ? &initialData : nullptr, m_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create structured buffer.");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(nrOfElementsInBuffer);

	hr = device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_SRV.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create shader resource view for structured buffer.");
	}
}

void StructuredBuffer::UpdateBuffer(ID3D11DeviceContext* context, void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRESULT hr = context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to map structured buffer for update.");
	}

	memcpy(mappedResource.pData, data, m_elementSize * m_nrOfElements);
	context->Unmap(m_buffer.Get(), 0);
}