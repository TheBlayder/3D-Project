#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	size_t m_nrOfIndices = 0;

public:
	IndexBuffer() = default;
	IndexBuffer(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData);
	~IndexBuffer() = default;
	IndexBuffer(const IndexBuffer& other) = delete;
	IndexBuffer& operator=(const IndexBuffer& other) = delete;
	IndexBuffer(IndexBuffer&& other) = delete;
	IndexBuffer& operator=(IndexBuffer&& other) = delete;

	void Init(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData);

	const size_t GetNrOfIndices() const;
	ID3D11Buffer* GetBuffer() const;
};