#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Transform.h"
#include "StructuredBuffer.h"
#include "ConstantBuffer.h"

namespace DX = DirectX;

struct LightData
{
	DX::XMFLOAT3 position;
	float intensity;
	DX::XMFLOAT4 color;
	DX::XMFLOAT3 direction;
	float padding;
};

struct LightBufferData
{
	int nrOfSpotLights;
	int nrOfDirLights;
	DX::XMFLOAT2 padding;
	DX::XMFLOAT3 cameraPosition;
	float padding2;
};

class LightHandler
{
private:
	std::vector<SpotLight> m_spotLights;
	std::vector<DirectionalLight> m_directionalLights;

	ConstantBuffer m_lightBuffer;
	LightBufferData m_lightBufferData;

	StructuredBuffer m_spotLightBuffer;
	StructuredBuffer m_directionalLightBuffer;


public:
	LightHandler() = default;
	~LightHandler() = default;
	
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition);

	void UpdateLightBuffer(ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition);

	void BindLightBuffer(ID3D11DeviceContext* context);

	// SpotLight
	void AddSpotLight(const SpotLight& spotLight);
	const std::vector<SpotLight>& GetSpotLights() const { return m_spotLights; }
	const size_t GetNrOfSpotLights() const { return m_spotLights.size(); }

	// DirectionalLight
	void AddDirectionalLight(const DirectionalLight& directionalLight);
	const std::vector<DirectionalLight>& GetDirectionalLights() const { return m_directionalLights; }
	const size_t GetNrOfDirectionalLights() const { return m_directionalLights.size(); }
};

