#include "ConstantBuffer.h"

#include <iostream>

ConstantBuffer::ConstantBuffer(ID3D11Device* device, size_t byteSize, void* initData)
{
	Init(device, byteSize, initData);
}

bool ConstantBuffer::Init(ID3D11Device* device, size_t byteSize, void* initData)
{
	m_size = static_cast<UINT>(byteSize);

	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth = m_size;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cBufferData;
	cBufferData.pSysMem = initData;
	cBufferData.SysMemPitch = 0;
	cBufferData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&cBufferDesc, &cBufferData, m_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Failed to create constant buffer!" << std::endl;
		return false;
	}

	return true;
}

ID3D11Buffer* ConstantBuffer::GetBuffer() const
{
	return m_buffer.Get();
}

ID3D11Buffer** ConstantBuffer::GetBufferPtr()
{
	return m_buffer.GetAddressOf();
}

const UINT ConstantBuffer::GetSize()
{
	return m_size;
}

void ConstantBuffer::Update(ID3D11DeviceContext* context, void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, data, m_size);
	context->Unmap(m_buffer.Get(), 0);
}
