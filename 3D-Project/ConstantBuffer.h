#pragma once

#include <d3d11.h>


class ConstantBuffer
{
private:
	ID3D11Buffer* m_buffer;
	UINT m_size;

public:
	ConstantBuffer() = default;
	ConstantBuffer(ID3D11Device* device, size_t byteSize, void* initData = nullptr);
	~ConstantBuffer();

	bool Init(ID3D11Device* device, size_t byteSize, void* initData = nullptr);

	ID3D11Buffer* GetBuffer() const;
	UINT GetSize() const;

	void Update(ID3D11DeviceContext* context, void* data, const UINT& size);
};

