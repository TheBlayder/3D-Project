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

struct SpotLightData
{
	DX::XMFLOAT3 position;
	float intensity;
	DX::XMFLOAT4 color;
	DX::XMFLOAT3 direction;
	float innerConeInDeg;
	float outerConeinDeg;
	float range;
	DX::XMFLOAT2 padding = { 0.f, 0.f };
	DX::XMFLOAT4X4 vpMatrix;
};

struct DirectionalLightData
{
	DX::XMFLOAT4 color;
	DX::XMFLOAT3 direction;
	float intensity;
	DX::XMFLOAT4X4 vpMatrix;
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
	ConstantBuffer m_lightBuffer;
	LightBufferData m_lightBufferData;
	
	std::vector<SpotLight> m_spotLights;
	StructuredBuffer m_spotLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_spotLightBufferSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spotLightDepthTex;
	D3D11_VIEWPORT m_spotLightViewport;
	void GetSpotLightData(std::vector<SpotLightData>& outData) const;

	std::vector<DirectionalLight> m_directionalLights;
	StructuredBuffer m_directionalLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_directionalLightBufferSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_directionalLightDepthTex;
	D3D11_VIEWPORT m_directionalLightViewport;
	void GetDirectionalLightData(std::vector<DirectionalLightData>& outData) const;

public:
	LightHandler(const UINT spotLightResolution, const UINT dirLightResolution);
	~LightHandler() = default;
	
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition);

	void UpdateLightBuffer(ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition);

	void BindLightBuffer(ID3D11DeviceContext* context);
	void UnbindLightBuffer(ID3D11DeviceContext* context);

	void BindDepthTextures(ID3D11DeviceContext* context);
	void UnbindDepthTextures(ID3D11DeviceContext* context);

	// SpotLight
	void AddSpotLight(SpotLightData& spotLight);
	const std::vector<SpotLight>& GetSpotLights() const { return m_spotLights; }
	const size_t GetNrOfSpotLights() const { return m_spotLights.size(); }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSpotLightBufferSRV() const { return m_spotLightBuffer.GetSRV(); }
	const D3D11_VIEWPORT& GetSpotLightViewport() const { return m_spotLightViewport; }

	// DirectionalLight
	void AddDirectionalLight(DirectionalLightData& dirLight);
	const std::vector<DirectionalLight>& GetDirectionalLights() const { return m_directionalLights; }
	const size_t GetNrOfDirectionalLights() const { return m_directionalLights.size(); }
	const D3D11_VIEWPORT& GetDirectionalLightViewport() const { return m_directionalLightViewport; }

};

