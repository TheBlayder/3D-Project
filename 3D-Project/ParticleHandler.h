#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>

#include "ConstantBuffer.h"
#include "ParticleBuffer.h"
#include "ReadCSO.h"
#include "Camera.h"

namespace DX = DirectX;

struct Particle
{
	DX::XMFLOAT3 position = { 0.f, 0.f, 0.f };
	float size = 0;
	DX::XMFLOAT3 velocity = { 0.f, 0.f, 0.f };
	float padding = 0;
	DX::XMFLOAT4 color = { 0.f, 0.f, 0.f , 0.f };
};

struct TimeBufferData
{
	float deltaTime = 0;
	DX::XMFLOAT3 padding = { 0.f, 0.f, 0.f };
};

struct CameraBufferData
{
	DX::XMFLOAT4X4 vpMatrix;
	DX::XMFLOAT3 cameraPosition = { 0.f, 0.f, 0.f };
	int isScenePaused = 0;
};

class ParticleHandler
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_particleVS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_particlePS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_particleGS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_particleCS = nullptr;

	ParticleBuffer m_particleBuffer;
	ConstantBuffer m_cameraBuffer;
	ConstantBuffer m_timeBuffer;

	void CreateShaders(ID3D11Device* device, ID3D11DeviceContext* context);

public:
	ParticleHandler() = default;
	~ParticleHandler() = default;

	void Init(ID3D11Device* device, ID3D11DeviceContext* context, UINT nrOfParticles, bool dynamic, bool hasSRV, bool hasUAV);

	void Update(ID3D11DeviceContext* context, const float deltaTime);
	void Draw(ID3D11DeviceContext* context, Camera* camera, D3D_PRIMITIVE_TOPOLOGY returnTopology, ID3D11InputLayout* returnInputLayout, const bool isScenePaused);

	// Getters
	ID3D11VertexShader* GetParticleVS() const { return m_particleVS.Get(); }
	ID3D11PixelShader* GetParticlePS() const { return m_particlePS.Get(); }
	ID3D11GeometryShader* GetParticleGS() const { return m_particleGS.Get(); }
	ID3D11ComputeShader* GetParticleCS() const { return m_particleCS.Get(); }

	ID3D11ShaderResourceView* GetParticleSRV() const { return m_particleBuffer.GetSRV(); }
	ID3D11UnorderedAccessView* GetParticleUAV() const { return m_particleBuffer.GetUAV(); }

	UINT GetNrOfParticles() const { return m_particleBuffer.GetNrOfParticles(); }
};