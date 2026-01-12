#include "Renderer.h"

#include "BaseObject.h"
#include "ReadCSO.h"

bool Renderer::Init(const Window& window)
{
	// Set up device and swapchain
	if(!CreateDeviceAndSwapChain(window)) return false;
	
	// Set up viewport
	CreateViewport(window);

	// Set up shaders
	std::string vShaderByteCode;
	if(!CreateShaders(vShaderByteCode)) return false;

	// Set up input layout
	if (!CreateInputLayout(vShaderByteCode)) return false;

	// Set up UAV
	if (!CreateUAV()) return false;

	// Set up sampler state
	if (!CreateSamplerStates()) return false;

	// Set up rasterizer state
	if (!CreateRasterizerStates()) return false;

	// Set up constant buffers
	if (!CreateConstantBuffers()) return false;

    return true;
}

bool Renderer::SetTesselation(const bool enable)
{
	if(enable && !m_tesselationEnabled)
	{
		m_tesselationEnabled = true;
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		m_immediateContext->IASetPrimitiveTopology(m_primitiveTopology);
		m_immediateContext->HSSetShader(m_hullShader.Get(), nullptr, 0);
		m_immediateContext->DSSetShader(m_domainShader.Get(), nullptr, 0);

		m_immediateContext->HSSetConstantBuffers(0, 1, m_tesselationBuffer.GetBufferPtr());
		m_immediateContext->DSSetConstantBuffers(0, 1, m_viewProjectionBuffer.GetBufferPtr());
	}
	else if(!enable && m_tesselationEnabled)
	{
		m_tesselationEnabled = false;
		m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_immediateContext->IASetPrimitiveTopology(m_primitiveTopology);
		m_immediateContext->HSSetShader(nullptr, nullptr, 0);
		m_immediateContext->DSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* nullBuf = nullptr;
		m_immediateContext->HSSetConstantBuffers(0, 1, &nullBuf);
		m_immediateContext->DSSetConstantBuffers(0, 1, &nullBuf);
	}

	return enable;
} 

void Renderer::RenderFrame(BaseScene* scene, const float deltaTime)
{
	scene->GetCamera()->GetDeferredHandler()->ClearBuffers(m_immediateContext.Get());

	// Update scene (camera, objects, lights, etc.)
	//scene->UpdateScene(deltaTime, m_immediateContext.Get(), &m_worldBuffer, &m_viewProjectionBuffer);

	this->ShadowPass(scene);	
	this->GeometryPass(scene);
	this->LightPass(scene);
	
	m_swapChain->Present(0, 0);
}


void Renderer::ShadowPass(BaseScene* scene)
{
	auto& sceneObjects = scene->GetSceneObjects();
	m_immediateContext->PSSetShader(nullptr, nullptr, 0); // No pixel shader for shadow pass

	m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_immediateContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetBufferPtr());
	m_immediateContext->VSSetConstantBuffers(1, 1, m_viewProjectionBuffer.GetBufferPtr());

	// Spot Lights
	m_immediateContext->RSSetViewports(1, &scene->GetLightHandler()->GetSpotLightViewport());
	const std::vector<SpotLight>& spotLights = scene->GetLightHandler()->GetSpotLights();
	for (auto& spotLight : spotLights)
	{
		m_immediateContext->ClearDepthStencilView(spotLight.GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_immediateContext->OMSetRenderTargets(0, nullptr, spotLight.GetDSV());

		// Set view-projection matrix from light's perspective
		DirectX::XMFLOAT4X4 lightViewProjMatrix = spotLight.GetViewProjMatrix();
		m_viewProjectionBuffer.Update(m_immediateContext.Get(), &lightViewProjMatrix);

		// Draw all game objects from the light's perspective
		for(auto& obj : sceneObjects)
		{
			DirectX::XMFLOAT4X4 worldMatrix = obj->GetWorldMatrix();
			m_worldBuffer.Update(m_immediateContext.Get(), &worldMatrix);

			obj->Draw(m_immediateContext.Get());
		}
	}

	// Directional Lights
	m_immediateContext->RSSetViewports(1, &scene->GetLightHandler()->GetDirectionalLightViewport());
	const std::vector<DirectionalLight>& dirLights = scene->GetLightHandler()->GetDirectionalLights();

	for(auto& dirLight : dirLights)
	{
		m_immediateContext->ClearDepthStencilView(dirLight.GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		m_immediateContext->OMSetRenderTargets(0, nullptr, dirLight.GetDSV());

		DirectX::XMFLOAT4X4 lightViewProjMatrix = dirLight.GetViewProjMatrix();
		m_viewProjectionBuffer.Update(m_immediateContext.Get(), &lightViewProjMatrix);

		for (auto& obj : sceneObjects)
		{
			DirectX::XMFLOAT4X4 worldMatrix = obj->GetWorldMatrix();
			m_worldBuffer.Update(m_immediateContext.Get(), &worldMatrix);
			obj->Draw(m_immediateContext.Get());
		}
	}

	// Reset states
	m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	m_immediateContext->RSSetViewports(1, &m_viewport);
}

void Renderer::GeometryPass(BaseScene* scene)
{
	m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Update camera constant buffer BEFORE drawing so vertex shader uses correct view-proj
	DirectX::XMFLOAT4X4 viewProjMatrix = scene->GetCamera()->GetViewProjMatrix();
	m_viewProjectionBuffer.Update(m_immediateContext.Get(), &viewProjMatrix); // Update viewProj matrix to viewProjectionBuffer
	m_immediateContext->VSSetConstantBuffers(1, 1, m_viewProjectionBuffer.GetBufferPtr());

	// Bind G-buffer render targets and depth
	scene->GetCamera()->GetDeferredHandler()->BindGeometryPass(m_immediateContext.Get());

	// Draw all game objects in the scene
	auto& sceneObjects = scene->GetSceneObjects();
	for (auto& obj : sceneObjects)
	{
		// Update world matrix constant buffer for each object
		DirectX::XMFLOAT4X4 worldMatrix = obj->GetWorldMatrix();
		m_worldBuffer.Update(m_immediateContext.Get(), &worldMatrix); // Update world matrix to worldBuffer
		m_immediateContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetBufferPtr());

		if(this->SetTesselation(obj->IsTesselationEnabled()) == true)
		{
			// Update tesselation constant buffer
			DirectX::XMFLOAT3 objectPos = obj->GetBoundingBox().Center;
			DirectX::XMVECTOR cameraPos = scene->GetCamera()->GetPosition();
			DirectX::XMVECTOR toObject = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&objectPos), cameraPos);
			m_tessData.distanceToObjectCenter = DirectX::XMVectorGetX(DirectX::XMVector3Length(toObject));

			m_tesselationBuffer.Update(m_immediateContext.Get(), &m_tessData);
		}

		obj->Draw(m_immediateContext.Get());
	}
	
	this->SetTesselation(false);
	scene->GetCamera()->GetDeferredHandler()->UnbindGeometryPass(m_immediateContext.Get());
}

void Renderer::LightPass(BaseScene* scene)
{

	// Bind G-buffers as SRVs and prepare UAV/backbuffer for output
	scene->GetCamera()->GetDeferredHandler()->BindLightPass(m_immediateContext.Get());

	// Bind UAV (backbuffer)
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, m_UAV.GetAddressOf(), nullptr);

	// Bind light sources
	scene->BindLights(m_immediateContext.Get());

	// Bind shadow maps
	scene->GetLightHandler()->BindDepthTextures(m_immediateContext.Get());

	// Compute dispatch
	const float threadGroupSizeXY = 8.f; // Must match [numthreads(x,y,z)] in compute shader
	UINT dispatchX = static_cast<UINT>(ceilf(m_viewport.Width / threadGroupSizeXY));
	UINT dispatchY = static_cast<UINT>(ceilf(m_viewport.Height / threadGroupSizeXY));
	m_immediateContext->Dispatch(dispatchX, dispatchY, 1);

	// Unbind compute shader and UAVs
	ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
	m_immediateContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	scene->GetLightHandler()->UnbindDepthTextures(m_immediateContext.Get());
}


void Renderer::CreateViewport(const Window& window)
{
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(window.GetWidth());
	m_viewport.Height = static_cast<FLOAT>(window.GetHeight());
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_immediateContext->RSSetViewports(1, &m_viewport);
}

bool Renderer::CreateDeviceAndSwapChain(const Window& window)
{
	UINT flags = 0;
	if (_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = window.GetWidth();
	swapChainDesc.BufferDesc.Height = window.GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = window.GetWindowHandle();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		nullptr,
		m_immediateContext.GetAddressOf()
	);

	if(FAILED(hr))
	{
		std::cerr << "Error creating device and swap chain!" << std::endl;
		return false;
	}

	return true;
}

bool Renderer::CreateShaders(std::string& vShaderByteCodeOUT)
{	
	// Vertex shader
	if(!CSOReader::ReadCSO("VertexShader.cso", vShaderByteCodeOUT))
	{
		std::cerr << "Error reading vertex shader bytecode!" << std::endl;
		return false;
	}
	HRESULT hr = m_device->CreateVertexShader(vShaderByteCodeOUT.data(), vShaderByteCodeOUT.size(), nullptr, m_vertexShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating vertex shader!" << std::endl;
		return false;
	}
	
	std::string byteCode; // Temporary storage for shader bytecode, gets cleared with each use in CSOReader
	// Pixel shader
	if(!CSOReader::ReadCSO("PixelShader.cso", byteCode))
	{
		std::cerr << "Error reading pixel shader bytecode!" << std::endl;
		return false;
	}
	hr = m_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, m_pixelShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating pixel shader!" << std::endl;
		return false;
	}

	// Compute shader
	if(!CSOReader::ReadCSO("ComputeShader.cso", byteCode))
	{
		std::cerr << "Error reading compute shader bytecode!" << std::endl;
		return false;
	}
	hr = m_device->CreateComputeShader(byteCode.data(), byteCode.size(), nullptr, m_computeShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating compute shader!" << std::endl;
		return false;
	}

	// DCEM Pixel Shader
	if (!CSOReader::ReadCSO("DCEM-PS.cso", byteCode))
	{
		std::cerr << "Error reading DCEM pixel shader bytecode!" << std::endl;
		return false;
	}
	hr = m_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, m_DCEMPixelShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating DCEM pixel shader!" << std::endl;
		return false;
	}

	// Hull Shader
	if (!CSOReader::ReadCSO("HullShader.cso", byteCode))
	{
		std::cerr << "Error reading hull shader bytecode!" << std::endl;
		return false;
	}
	hr = m_device->CreateHullShader(byteCode.data(), byteCode.size(), nullptr, m_hullShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating hull shader!" << std::endl;
		return false;
	}

	// Domain Shader
	if (!CSOReader::ReadCSO("DomainShader.cso", byteCode))
	{
		std::cerr << "Error reading domain shader bytecode!" << std::endl;
		return false;
	}
	hr = m_device->CreateDomainShader(byteCode.data(), byteCode.size(), nullptr, m_domainShader.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating domain shader!" << std::endl;
		return false;
	}

	m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	m_immediateContext->CSSetShader(m_computeShader.Get(), nullptr, 0);

	return true;
}

bool Renderer::CreateInputLayout(const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0} 
	};

	HRESULT hr = m_device->CreateInputLayout(
		layoutDesc,
		ARRAYSIZE(layoutDesc),
		vShaderByteCode.data(),
		vShaderByteCode.size(),
		m_inputLayout.GetAddressOf()
	);

	if(FAILED(hr))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	m_immediateContext->IASetInputLayout(m_inputLayout.Get());
	m_immediateContext->IASetPrimitiveTopology(m_primitiveTopology);

	return true;
}

bool Renderer::CreateUAV()
{	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)backBuffer.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error getting back buffer!" << std::endl;
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.ArraySize = 1;

	hr = m_device->CreateUnorderedAccessView(backBuffer.Get(), &uavDesc, m_UAV.GetAddressOf());
	if (FAILED(hr))
	{
		std::cerr << "Error creating UAV!" << std::endl;
		return false;
	}

	return true;
}

bool Renderer::CreateSamplerStates()
{
	// General texture sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating sampler state!" << std::endl;
		return false;
	}

	m_immediateContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	// Shadow map sampler state
	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.MipLODBias = 0.0f;
	shadowSamplerDesc.MaxAnisotropy = 16;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	shadowSamplerDesc.MinLOD = 0;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&shadowSamplerDesc, m_shadowSamplerState.GetAddressOf());

	if(FAILED(hr))
	{
		std::cerr << "Error creating shadow sampler state!" << std::endl;
		return false;
	}

	m_immediateContext->CSSetSamplers(0, 1, m_shadowSamplerState.GetAddressOf());

	return true;
}

bool Renderer::CreateRasterizerStates()
{
	// Default rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;

	HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, m_defaultRasterizerState.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating rasterizer state!" << std::endl;
		return false;
	}

	// Wireframe rasterizer state
	D3D11_RASTERIZER_DESC wireframeDesc = {};
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = FALSE;
	wireframeDesc.DepthClipEnable = TRUE;

	hr = m_device->CreateRasterizerState(&wireframeDesc, m_wireframeRasterizerState.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating wireframe rasterizer state!" << std::endl;
		return false;
	}

	m_immediateContext->RSSetState(m_defaultRasterizerState.Get());
	return true;
}

bool Renderer::CreateConstantBuffers()
{
	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());

	// World matrix buffer
	if (!m_worldBuffer.Init(m_device.Get(), sizeof(DirectX::XMFLOAT4X4), &identityMatrix))
	{
		std::cerr << "Error creating world constant buffer!" << std::endl;
		return false;
	}

	// ViewProjection matrix buffer
	if (!m_viewProjectionBuffer.Init(m_device.Get(), sizeof(DirectX::XMFLOAT4X4), &identityMatrix))
	{
		std::cerr << "Error creating viewProjection constant buffer!" << std::endl;
		return false;
	}

	// Tesselation buffer
	if(!m_tesselationBuffer.Init(m_device.Get(), sizeof(TesselationData), &m_tessData))
	{
		std::cerr << "Error creating tesselation constant buffer!" << std::endl;
		return false;
	}

	m_immediateContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetBufferPtr());
	m_immediateContext->VSSetConstantBuffers(1, 1, m_viewProjectionBuffer.GetBufferPtr());

	return true;
}

void Renderer::SetWireframe(const bool enable)
{
	m_showWireframe = enable;

	if (m_showWireframe)
	{
		m_immediateContext->RSSetState(m_wireframeRasterizerState.Get());
	}
	else
	{
		m_immediateContext->RSSetState(m_defaultRasterizerState.Get());
	}
}