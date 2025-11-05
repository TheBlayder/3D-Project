#pragma once

#include <cstdint>
#include <d3d11.h>

class IndexBuffer
{
private:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_nrOfIndices = 0;

public:
	IndexBuffer() = default;
	IndexBuffer(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData);
	~IndexBuffer();
	IndexBuffer(const IndexBuffer& other) = delete;
	IndexBuffer& operator=(const IndexBuffer& other) = delete;
	IndexBuffer(IndexBuffer&& other) = delete;
	IndexBuffer& operator=(IndexBuffer&& other) = delete;

	void Init(ID3D11Device* device, size_t nrOfIndicesInBuffer, uint32_t* indexData);

	size_t GetNrOfIndices() const;
	ID3D11Buffer* GetBuffer() const;
};