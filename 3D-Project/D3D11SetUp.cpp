#include "D3D11SetUp.h"

D3D11SetUp::D3D11SetUp()
{
}

D3D11SetUp::~D3D11SetUp()
{
}

bool D3D11SetUp::SetUp(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, 
	ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, Window*& window)
{
	if (!CreateSwapChain(device, immediateContext, swapChain, window))
	{
		std::cerr << "Error creating Swap Chain!" << std::endl;
		return false;
	}

	if (!CreateRenderTargetView(device, swapChain, rtv))
	{
		std::cerr << "Error creating Render Target View!" << std::endl;
		return false;
	}

	if (!CreateDepthStencilView(device, window, dsTexture, dsView))
	{
		std::cerr << "Error creating Depth Stencil View!" << std::endl;
		return false;
	}

	SetViewPort(viewport, window);

	return true;
}

bool D3D11SetUp::CreateSwapChain(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, Window* window)
{
	UINT flags = 0;
	if(_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	// Create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = window->GetWidth();
	swapChainDesc.BufferDesc.Height = window->GetHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 165;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = window->GetWindowHandle();
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
		&swapChain,
		&device,
		nullptr,
		&immediateContext
	);

	return SUCCEEDED(hr);
}

bool D3D11SetUp::CreateRenderTargetView(ID3D11Device*& device, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv)
{
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	return SUCCEEDED(hr);
}

bool D3D11SetUp::CreateDepthStencilView(ID3D11Device*& device, Window* window, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = window->GetWidth();
	depthDesc.Height = window->GetHeight();
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&depthDesc, nullptr, &dsTexture);
	if (FAILED(hr))
	{
		std::cerr << "Error creating depth stencil texture!" << std::endl;
		return false;
	}

	hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
	if (FAILED(hr))
	{
		std::cerr << "Error creating depth stencil view!" << std::endl;
		return false;
	}

	return true;
}

void D3D11SetUp::SetViewPort(D3D11_VIEWPORT& viewport, Window*& window)
{
	viewport.Width    = static_cast<float>(window->GetWidth());
	viewport.Height   = static_cast<float>(window->GetHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}
