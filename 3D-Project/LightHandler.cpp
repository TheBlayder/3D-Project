#include "LightHandler.h"

void LightHandler::UpdateLightBuffer(ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition)
{
	m_lightBufferData.cameraPosition = cameraPosition;
	m_lightBuffer.Update(context, &m_lightBufferData);
}

void LightHandler::BindLightBuffer(ID3D11DeviceContext* context)
{
	context->CSSetConstantBuffers(0, 1, m_lightBuffer.GetBufferPtr());
	
	// Only bind valid SRVs
	if (m_spotLightBuffer.GetSRV() && m_directionalLightBuffer.GetSRV())
	{
		ID3D11ShaderResourceView* SRVs[2] = { m_spotLightBuffer.GetSRV(), m_directionalLightBuffer.GetSRV() };
		context->CSSetShaderResources(5, 2, SRVs);
	}
	else if (m_spotLightBuffer.GetSRV())
	{
		context->CSSetShaderResources(5, 1, m_spotLightBuffer.GetSRVPtr());
	}
	else if (m_directionalLightBuffer.GetSRV())
	{
		context->CSSetShaderResources(6, 1, m_directionalLightBuffer.GetSRVPtr());
	}
}

void LightHandler::UnbindLightBuffer(ID3D11DeviceContext* context)
{
	ID3D11Buffer* nullBuffer = nullptr;
	context->CSSetConstantBuffers(0, 1, &nullBuffer);

	ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
	context->CSSetShaderResources(5, 2, nullSRVs);
}

void LightHandler::AddSpotLight(SpotLightData& spotLight)
{
	m_spotLights.emplace_back(spotLight.position, spotLight.color, spotLight.direction, 
		spotLight.intensity, spotLight.innerConeInDeg, spotLight.outerConeinDeg, spotLight.range);
}

void LightHandler::AddDirectionalLight(DirectionalLightData& dirLight)
{
	m_directionalLights.emplace_back(dirLight.color, dirLight.direction, dirLight.intensity);
}

void LightHandler::GetSpotLightData(std::vector<SpotLightData>& outData) const
{
	outData.reserve(m_spotLights.size());
	for (const auto& spotLight : m_spotLights)
	{
		SpotLightData data = {};
		data.position = spotLight.GetPosition();
		data.intensity = spotLight.GetIntensity();
		data.color = spotLight.GetColor();
		data.direction = spotLight.GetDirection();
		data.innerConeInDeg = spotLight.GetInnerConeInDeg();
		data.outerConeinDeg = spotLight.GetOuterConeInDeg();
		data.range = spotLight.GetRange();

		outData.emplace_back(data);
	}
}

void LightHandler::GetDirectionalLightData(std::vector<DirectionalLightData>& outData) const
{
	outData.reserve(m_directionalLights.size());
	for (const auto& dirLight : m_directionalLights)
	{
		DirectionalLightData data = {};
		data.color = dirLight.GetColor();
		data.direction = dirLight.GetDirection();
		data.intensity = dirLight.GetIntensity();
		
		outData.emplace_back(data);
	}
}

/// <summary>
/// ONLY TO BE CALLED AFTER ALL LIGHTS HAVE BEEN ADDED
/// </summary>
void LightHandler::Init(ID3D11Device* device, ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition)
{
    // Spot lights buffer
    std::vector<SpotLightData> spotLightData;
    GetSpotLightData(spotLightData);
    if (!spotLightData.empty())
        m_spotLightBuffer.Init(device, sizeof(SpotLightData), spotLightData.size(), spotLightData.data());

    // Directional lights buffer
    std::vector<DirectionalLightData> dirLightData;
    GetDirectionalLightData(dirLightData);
    if (!dirLightData.empty())  // Add this check
        m_directionalLightBuffer.Init(device, sizeof(DirectionalLightData), dirLightData.size(), dirLightData.data());
    
    // Initialize light buffer metadata
	m_lightBufferData = {};
	m_lightBufferData.nrOfSpotLights = static_cast<int>(m_spotLights.size());
	m_lightBufferData.nrOfDirLights = static_cast<int>(m_directionalLights.size());
	m_lightBufferData.cameraPosition = cameraPosition;
	
	m_lightBuffer.Init(device, sizeof(LightBufferData), &m_lightBufferData);

	BindLightBuffer(context);
}
