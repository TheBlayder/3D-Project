#pragma once

#include <d3d11.h>


class ConstantBuffer
{
private:
	ID3D11Buffer* m_buffer;
	size_t m_size;

public:
	ConstantBuffer() = default;
	ConstantBuffer(ID3D11Device* device, size_t byteSize, void* initData = nullptr);
	~ConstantBuffer();

	bool Init(ID3D11Device* device, size_t byteSize, void* initData = nullptr);

	ID3D11Buffer* GetBuffer() const;
	size_t GetSize() const;

	void Update(ID3D11DeviceContext* context, void* data);
};

