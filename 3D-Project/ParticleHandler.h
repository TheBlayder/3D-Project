#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "ParticleBuffer.h"
#include "ReadCSO.h"

namespace DX = DirectX;

struct Particle
{
	DX::XMFLOAT3 position;
	float size;
	DX::XMFLOAT3 velocity;
	float padding;
	DX::XMFLOAT4 color;
};

class ParticleHandler
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_particleVS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_particlePS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_particleGS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_particleCS = nullptr;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	ParticleBuffer m_particleBuffer;

	void CreateShaders(ID3D11Device* device, ID3D11DeviceContext* context, std::string& vShaderByteCode);
	void CreateInputLayout(ID3D11Device* device, const std::string& vShaderByteCode, UINT particleSize);

public:
	ParticleHandler() = default;
	~ParticleHandler() = default;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context, UINT particleSize, UINT nrOfParticles, bool dynamic, bool hasSRV, bool hasUAV);

	// Getters
	ID3D11VertexShader* GetParticleVS() const { return m_particleVS.Get(); }
	ID3D11PixelShader* GetParticlePS() const { return m_particlePS.Get(); }
	ID3D11GeometryShader* GetParticleGS() const { return m_particleGS.Get(); }
	ID3D11ComputeShader* GetParticleCS() const { return m_particleCS.Get(); }

	ID3D11ShaderResourceView* GetParticleSRV() const { return m_particleBuffer.GetSRV(); }
	ID3D11UnorderedAccessView* GetParticleUAV() const { return m_particleBuffer.GetUAV(); }

	UINT GetNrOfParticles() const { return m_particleBuffer.GetNrOfParticles(); }
};