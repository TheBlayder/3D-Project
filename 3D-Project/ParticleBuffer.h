#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class ParticleBuffer {
private:
	UINT m_particleSize = 0;
	UINT m_nrOfParticles = 0;
	bool m_isDynamic = false;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV = nullptr;

public:
	ParticleBuffer() = default;
	ParticleBuffer(ID3D11Device* device, UINT size, UINT nrOf, bool dynamic, bool hasSRV, bool hasUAV, void* initData);
	~ParticleBuffer() = default;

	void Init(ID3D11Device* device, UINT size, UINT nrOf, bool dynamic, bool hasSRV, bool hasUAV, void* initData);

	ID3D11Buffer* GetBuffer() const { return m_buffer.Get(); }
	ID3D11ShaderResourceView* GetSRV() const { return m_SRV.Get(); }
	ID3D11UnorderedAccessView* GetUAV() const { return m_UAV.Get(); }
};