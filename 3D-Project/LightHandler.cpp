#include "LightHandler.h"

LightHandler::LightHandler(const UINT spotLightResolution, const UINT dirLightResolution)
{
	// Spot light viewport
	m_spotLightViewport.TopLeftX = 0.f;
	m_spotLightViewport.TopLeftY = 0.f;
	m_spotLightViewport.Width = static_cast<FLOAT>(spotLightResolution);
	m_spotLightViewport.Height = static_cast<FLOAT>(spotLightResolution);
	m_spotLightViewport.MinDepth = 0.f;
	m_spotLightViewport.MaxDepth = 1.f;

	// Directional light viewport
	m_directionalLightViewport.TopLeftX = 0.f;
	m_directionalLightViewport.TopLeftY = 0.f;
	m_directionalLightViewport.Width = static_cast<FLOAT>(dirLightResolution);
	m_directionalLightViewport.Height = static_cast<FLOAT>(dirLightResolution);
	m_directionalLightViewport.MinDepth = 0.f;
	m_directionalLightViewport.MaxDepth = 1.f;
}

void LightHandler::UpdateLightBuffer(ID3D11DeviceContext* context, const DX::XMFLOAT3 cameraPosition)
{
	m_lightBufferData.cameraPosition = cameraPosition;
	m_lightBuffer.Update(context, &m_lightBufferData);
}

void LightHandler::BindLightBuffer(ID3D11DeviceContext* context)
{
	context->CSSetConstantBuffers(0, 1, m_lightBuffer.GetBufferPtr());
	
	ID3D11ShaderResourceView* SRVs[2] = { m_spotLightBuffer.GetSRV(), m_directionalLightBuffer.GetSRV() };
	context->CSSetShaderResources(5, 2, SRVs);
}

void LightHandler::UnbindLightBuffer(ID3D11DeviceContext* context)
{
	ID3D11Buffer* nullBuffer = nullptr;
	context->CSSetConstantBuffers(0, 1, &nullBuffer);

	ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
	context->CSSetShaderResources(5, 2, nullSRVs);
}

void LightHandler::BindDepthTextures(ID3D11DeviceContext* context)
{
    ID3D11ShaderResourceView* SRVs[2] = {
        m_spotLightBufferSRV.Get(),
        m_directionalLightBufferSRV.Get()
    };
    context->CSSetShaderResources(7, 2, SRVs);
}

void LightHandler::UnbindDepthTextures(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
	context->CSSetShaderResources(7, 2, nullSRVs);
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
		data.vpMatrix = spotLight.GetViewProjMatrix();

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
		data.vpMatrix = dirLight.GetViewProjMatrix();
		
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

	// Set up Shadow Map
	if(!m_spotLights.empty())
	{
		D3D11_TEXTURE2D_DESC spotLightDesc = {};
		spotLightDesc.Width = m_spotLightViewport.Width;
		spotLightDesc.Height = m_spotLightViewport.Height;
		spotLightDesc.MipLevels = 1;
		spotLightDesc.ArraySize = static_cast<UINT>(m_spotLights.size());
		spotLightDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		spotLightDesc.SampleDesc.Count = 1;
		spotLightDesc.SampleDesc.Quality = 0;
		spotLightDesc.Usage = D3D11_USAGE_DEFAULT;
		spotLightDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		spotLightDesc.CPUAccessFlags = 0;
		spotLightDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&spotLightDesc, nullptr, m_spotLightDepthTex.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Could not create depth buffer texture");
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.ArraySize = 1;

		for (size_t dsvIndex = 0; dsvIndex < m_spotLights.size(); dsvIndex++)
		{
			dsvDesc.Texture2DArray.FirstArraySlice = dsvIndex;
			hr = m_spotLights[dsvIndex].Init(device, m_spotLightDepthTex.Get(), &dsvDesc);
			if (FAILED(hr))
			{
				throw std::runtime_error("Could not create depth buffer dsv");
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(m_spotLights.size());

		hr = device->CreateShaderResourceView(m_spotLightDepthTex.Get(), &srvDesc, m_spotLightBufferSRV.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Could not create depth buffer srv");
		}
	}

	if(!m_directionalLights.empty())
	{
		D3D11_TEXTURE2D_DESC dirLightDesc = {};
		dirLightDesc.Width = m_directionalLightViewport.Width;
		dirLightDesc.Height = m_directionalLightViewport.Height;
		dirLightDesc.MipLevels = 1;
		dirLightDesc.ArraySize = static_cast<UINT>(m_directionalLights.size());
		dirLightDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		dirLightDesc.SampleDesc.Count = 1;
		dirLightDesc.SampleDesc.Quality = 0;
		dirLightDesc.Usage = D3D11_USAGE_DEFAULT;
		dirLightDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		dirLightDesc.CPUAccessFlags = 0;
		dirLightDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&dirLightDesc, nullptr, m_directionalLightDepthTex.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Could not create depth buffer texture");
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.ArraySize = 1;

		for (size_t dsvIndex = 0; dsvIndex < m_directionalLights.size(); dsvIndex++)
		{
			dsvDesc.Texture2DArray.FirstArraySlice = dsvIndex;
			hr = m_directionalLights[dsvIndex].Init(device, m_directionalLightDepthTex.Get(), &dsvDesc);
			if (FAILED(hr))
			{
				throw std::runtime_error("Could not create depth buffer dsv");
			}
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = static_cast<UINT>(m_directionalLights.size());

		hr = device->CreateShaderResourceView(m_directionalLightDepthTex.Get(), &srvDesc, m_directionalLightBufferSRV.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Could not create depth buffer srv");
		}
	}

	BindLightBuffer(context);
}
