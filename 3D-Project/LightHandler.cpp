#include "LightHandler.h"

void LightHandler::UpdateLightBuffer(ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition)
{
	m_lightBufferData.cameraPosition = cameraPosition;
	m_lightBuffer.Update(context, &m_lightBufferData);
}

void LightHandler::BindLightBuffer(ID3D11DeviceContext* context)
{
	context->CSSetConstantBuffers(0, 1, m_lightBuffer.GetBufferPtr());
	
	ID3D11ShaderResourceView* SRVs[2] = { m_spotLightBuffer.GetSRV(), m_directionalLightBuffer.GetSRV() };
	context->CSSetShaderResources(5, 2, SRVs); // Shader slots 5 and 6, match with compute shader
}

/// <summary>
/// ONLY TO BE CALLED AFTER ALL LIGHTS HAVE BEEN ADDED
/// </summary>
void LightHandler::Init(ID3D11Device* device, ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition)
{
	m_spotLightBuffer.Init(device, sizeof(LightData), m_spotLights.size(), m_spotLights.data(), true);
	m_directionalLightBuffer.Init(device, sizeof(LightData), m_directionalLights.size(), m_directionalLights.data(), true);

	m_lightBufferData = {};
	m_lightBufferData.nrOfSpotLights = static_cast<int>(m_spotLights.size());
	m_lightBufferData.nrOfDirLights = static_cast<int>(m_directionalLights.size());
	m_lightBufferData.cameraPosition = cameraPosition;
	
	m_lightBuffer.Init(device, sizeof(LightBufferData), &m_lightBufferData);

	BindLightBuffer(context);
}
