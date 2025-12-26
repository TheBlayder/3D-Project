#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <stdexcept>

class StructuredBuffer

{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;
	UINT m_elementSize = 0;
	size_t m_nrOfElements = 0;

public:
	StructuredBuffer() = default;
	StructuredBuffer(ID3D11Device* device, UINT sizeOfElement,
		size_t nrOfElementsInBuffer, void* bufferData = nullptr, bool dynamic = true);
	~StructuredBuffer() = default;
	StructuredBuffer(const StructuredBuffer& other) = delete;
	StructuredBuffer& operator=(const StructuredBuffer& other) = delete;
	StructuredBuffer(StructuredBuffer&& other) = delete;
	StructuredBuffer operator=(StructuredBuffer&& other) = delete;

	void Init(ID3D11Device* device, UINT sizeOfElement,
		size_t nrOfElementsInBuffer, void* bufferData = nullptr, bool dynamic = true);

	void UpdateBuffer(ID3D11DeviceContext* context, void* data);

	UINT GetElementSize() const { return m_elementSize; }
	size_t GetNrOfElements() const { return m_nrOfElements; }
	ID3D11ShaderResourceView* GetSRV() const { return m_SRV.Get(); }
};