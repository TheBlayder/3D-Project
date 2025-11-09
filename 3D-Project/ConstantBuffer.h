#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class ConstantBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_size = 0;

public:
	ConstantBuffer() = default;
	ConstantBuffer(ID3D11Device* device, size_t byteSize, void* initData = nullptr);
	~ConstantBuffer() = default;

	bool Init(ID3D11Device* device, size_t byteSize, void* initData = nullptr);

	ID3D11Buffer* GetBuffer() const;
	ID3D11Buffer** GetBufferPtr();
	const UINT GetSize();

	void Update(ID3D11DeviceContext* context, void* data);
};

