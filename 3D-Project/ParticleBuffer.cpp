#include "ParticleBuffer.h"

#include <stdexcept>


ParticleBuffer::ParticleBuffer(ID3D11Device* device, UINT size, UINT nrOf, bool dynamic, bool hasSRV, bool hasUAV, void* initData)
{
	Init(device, size, nrOf, dynamic, hasSRV, hasUAV, initData);
}

void ParticleBuffer::Init(ID3D11Device* device, UINT size, UINT nrOf, bool dynamic, bool hasSRV, bool hasUAV, void* initData)
{
	if(size == 0 || nrOf == 0)
		throw std::runtime_error("ParticleBuffer size and number of particles must be greater than zero.");

	if(initData == nullptr)
		throw std::runtime_error("ParticleBuffer buffer must have initial data.");

	m_particleSize = size;
	m_nrOfParticles = nrOf;
	m_isDynamic = dynamic;
	
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = size * nrOf;
	bufferDesc.StructureByteStride = size;
	bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = (hasSRV ? D3D11_BIND_SHADER_RESOURCE : static_cast<UINT>(0)) | (hasUAV ? D3D11_BIND_UNORDERED_ACCESS : static_cast<UINT>(0));
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : static_cast<UINT>(0);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	
	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = initData;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &initialData, m_buffer.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("Failed to create particle Structured-buffer.");
	
	if(hasSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = nrOf;

		hr = device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_SRV.GetAddressOf());
		if(FAILED(hr))
			throw std::runtime_error("Failed to create Particle Shader Resource View.");
	}
	if(hasUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = nrOf;
		uavDesc.Buffer.Flags = 0;

		hr = device->CreateUnorderedAccessView(m_buffer.Get(), &uavDesc, m_UAV.GetAddressOf());
		if(FAILED(hr))
			throw std::runtime_error("Failed to create Particle Unordered Access View.");
	}
}
