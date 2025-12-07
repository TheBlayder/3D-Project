#include "Renderer.h"
#include "ReadCSO.h"

#include "Transform.h"

#include "TestObject.h"

Renderer::~Renderer()
{
    delete m_test1;
    delete m_camera;
}

bool Renderer::Init(const Window& window)
{
	// Set up device and swapchain
	if(!CreateDeviceAndSwapChain(window)) return false;
	
	// Set up viewport
	CreateViewport(window);

	if (!CreateRenderTargetView()) return false;

	if (!CreateDepthStencilView(window)) return false;

	// Set up shaders
	std::string vShaderByteCode;
	if(!CreateShaders(vShaderByteCode)) return false;

	// Set up input layout
	if (!CreateInputLayout(vShaderByteCode)) return false;

	// Set up UAV
	//if (!CreateUAV()) return false;

	// Set up sampler state
	if (!CreateSamplerState()) return false;

	// Set up rasterizer state
	if (!CreateRasterizerState()) return false;

	// Set up constant buffers
	if (!CreateConstantBuffers()) return false;

	// Cube test object
	Transform testTransform;
	std::string folderPath = "./Objects/Cube";
	std::string objectName = "cube.obj";
	testTransform.SetPosition(DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f));
	testTransform.SetRotation(DirectX::XMVectorSet(0.5f, 0.5f, 0.0f, 0.0f));
	testTransform.SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	m_test1 = new GameObject(m_device.Get(), testTransform, folderPath, objectName);

	//// Create a simple triangle test object
	//m_test1 = new TestObject(m_device.Get());

	// Camera
	DirectX::XMFLOAT3 camInitialPos = { 0.0f, 0.0f, -3.0f };
	ProjectionData projData;
	projData.fovInDeg = 70.0f;
	projData.aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 1000.0f;

	m_camera = new Camera(m_device.Get(), projData, camInitialPos);

    return true;
}

//void Renderer::Render(BaseScene* scene, float deltaTime)
//{
//
//}

// For testing purposes
void Renderer::RenderFrame()
{
	m_immediateContext->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());

	float clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_immediateContext->ClearRenderTargetView(m_rtv.Get(), clearColor);
	m_immediateContext->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Update constant buffers
	DirectX::XMFLOAT4X4 worldMatrix = m_test1->GetWorldMatrix();
	m_worldBuffer.Update(m_immediateContext.Get(), &worldMatrix); // Update world matrix to worldBuffer

	DirectX::XMFLOAT4X4 viewProjMatrix = m_camera->GetViewProjMatrix();
	m_viewProjectionBuffer.Update(m_immediateContext.Get(), &viewProjMatrix); // Update viewProj matrix to viewProjectionBuffer

	// Bind VS constant buffers
	m_immediateContext->VSSetConstantBuffers(0, 1, m_worldBuffer.GetBufferPtr()); // Set world buffer
	m_immediateContext->VSSetConstantBuffers(1, 1, m_viewProjectionBuffer.GetBufferPtr()); // Set viewProjection buffer

	// Draw test object
	m_test1->Draw(m_immediateContext.Get());

	// Unbind shader resource views (safety — avoids "resource still bound as SRV" on next writes)
	ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
	m_immediateContext->PSSetShaderResources(0, 3, nullSRVs);

	// Present the rendered frame to the screen
	m_swapChain->Present(0, 0);
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

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = window.GetWidth();
	swapChainDesc.BufferDesc.Height = window.GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //  | DXGI_USAGE_UNORDERED_ACCESS
	swapChainDesc.BufferCount = 1; // 2
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

	m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

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

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = m_device->CreateUnorderedAccessView(backBuffer.Get(), &uavDesc, m_uav.GetAddressOf());
	return true;
}

bool Renderer::CreateRenderTargetView()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to get back buffer! hr=" << std::hex << hr << std::endl;
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_rtv.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create RTV! hr=" << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

bool Renderer::CreateDepthStencilView(const Window& window)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.Width = window.GetWidth();
	depthStencilDesc.Height = window.GetHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HRESULT hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, m_depthStencilBuffer.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating depth stencil buffer!" << std::endl;
		return false;
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_dsv.GetAddressOf());
	if(FAILED(hr))
	{
		std::cerr << "Error creating depth stencil view!" << std::endl;
		return false;
	}

	return true;
}

bool Renderer::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

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
	return !FAILED(hr);
}

bool Renderer::CreateRasterizerState()
{
	// Default rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

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
	return true;
}

ID3D11Device* Renderer::GetDevice()
{
	return m_device.Get();
}

ID3D11DeviceContext* Renderer::GetImmediateContext()
{
	return m_immediateContext.Get();
}
