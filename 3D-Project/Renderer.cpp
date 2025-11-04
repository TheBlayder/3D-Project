#include "Renderer.h"
#include "ReadCSO.h"

void Renderer::CreateViewport(const Window& window)
{
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(window.GetWidth());
	viewport.Height = static_cast<FLOAT>(window.GetHeight());
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_immediateContext->RSSetViewports(1, &viewport);
}

bool Renderer::CreateDeviceAndSwapChain(const Window& window)
{
	UINT flags = 0;
	if (_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = window.GetWidth();
	swapChainDesc.BufferDesc.Height = window.GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 165;
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
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_immediateContext
	);
	return SUCCEEDED(hr);
}

bool Renderer::CreateShaders(std::string& vShaderByteCodeOUT)
{
	// Vertex shader
	if(!CSOReader::ReadCSO("VertexShader.cso", vShaderByteCodeOUT))
	{
		std::cerr << "Error reading vertex shader bytecode!" << std::endl;
		return false;
	}
	
	HRESULT hr = m_device->CreateVertexShader(vShaderByteCodeOUT.c_str(), vShaderByteCodeOUT.length(), nullptr, &m_vertexShader);
	
	if(FAILED(hr))
	{
		std::cerr << "Error creating vertex shader!" << std::endl;
		return false;
	}
	
	std::string byteCode; // Temporary storage for shader bytecode
	// Pixel shader
	if(!CSOReader::ReadCSO("PixelShader.cso", byteCode))
	{
		std::cerr << "Error reading pixel shader bytecode!" << std::endl;
		return false;
	}

	hr = m_device->CreatePixelShader(byteCode.c_str(), byteCode.length(), nullptr, &m_pixelShader);

	if(FAILED(hr))
	{
		std::cerr << "Error creating pixel shader!" << std::endl;
		return false;
	}
	byteCode.clear(); // Clear bytecode for reuse


	// Compute shader
	if(!CSOReader::ReadCSO("ComputeShader.cso", byteCode))
	{
		std::cerr << "Error reading compute shader bytecode!" << std::endl;
		return false;
	}

	hr = m_device->CreateComputeShader(byteCode.c_str(), byteCode.length(), nullptr, &m_computeShader);

	if(FAILED(hr))
	{
		std::cerr << "Error creating compute shader!" << std::endl;
		return false;
	}
	byteCode.clear();

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
		vShaderByteCode.c_str(),
		vShaderByteCode.length(),
		&m_inputLayout
	);

	if(FAILED(hr))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	m_immediateContext->IASetInputLayout(m_inputLayout);
	m_immediateContext->IASetPrimitiveTopology(m_primitiveTopology);

	return true;
}

bool Renderer::CreateUAV()
{
	return false;
}

bool Renderer::CreateSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
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

	HRESULT hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
	return !FAILED(hr);
}

bool Renderer::CreateRasterizerState()
{
	// Default rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;

	HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_defaultRasterizerState);

	if(FAILED(hr))
	{
		std::cerr << "Error creating rasterizer state!" << std::endl;
		return false;
	}

	m_immediateContext->RSSetState(m_defaultRasterizerState);
	return true;
}

Renderer::Renderer() {}

Renderer::~Renderer() {}

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
	if (!CreateSamplerState()) return false;

	// Set up rasterizer state
	if (!CreateRasterizerState()) return false;

    return true;
}

ID3D11Device* Renderer::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* Renderer::GetImmediateContext()
{
	return m_immediateContext;
}

