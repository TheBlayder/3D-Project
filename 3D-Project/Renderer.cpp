#include "Renderer.h"

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

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

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
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		nullptr,
		&m_immediateContext
	);
	return SUCCEEDED(hr);
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

bool Renderer::Init(const Window& window)
{
	// Set up viewport
	CreateViewport(window);

	// Set up device and swapchain
	if(!CreateDeviceAndSwapChain(window))
	{
		std::cerr << "Error creating device and swapchain!" << std::endl;
		return false;
	}

	// Set up input layout


	// Set up UAV





    return false;
}

ID3D11Device* Renderer::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* Renderer::GetImmediateContext()
{
	return m_immediateContext;
}

