#include "EngineHandler.h"

// *** Tänk över struktur och hur jag vill ha det i framtiden.
// *** Kanske kasta undantag istället för att returnera false?

EngineHandler::EngineHandler(HINSTANCE hInstance, int nCmdShow, const UINT height, const UINT width)
{
	m_window = new Window(hInstance, nCmdShow, height, width);
	if (!m_window)
	{
		std::cerr << "Error setting up window!" << std::endl;
		return;
	}
	m_d3d11SetUp = new D3D11SetUp();
	if (!m_d3d11SetUp->SetUp(m_device, m_immediateContext, m_swapChain, m_rtv, m_dsTexture, m_dsView, m_viewport, m_window))
	{
		std::cerr << "Error setting up D3D11!" << std::endl;
		return;
	}
	m_pipelineSetUp = new PipelineSetUp();
	if (!m_pipelineSetUp->SetUp(m_device, m_immediateContext, m_vertexBuffer, m_vShader, m_pShader, m_inputLayout, m_VScBuffer, m_PScBuffer, m_texture, m_SRVTexture, m_samplerState, m_textureData))
	{
		std::cerr << "Error setting up pipeline!" << std::endl;
		return;
	}
}

EngineHandler::~EngineHandler()
{
	// Release D3D11 objects
	m_PScBuffer->Release();
	m_VScBuffer->Release();
	m_samplerState->Release();
	m_SRVTexture->Release();
	m_texture->Release();
	m_vertexBuffer->Release();
	m_inputLayout->Release();
	m_pShader->Release();
	m_vShader->Release();
	m_dsView->Release();
	m_dsTexture->Release();
	m_rtv->Release();
	m_swapChain->Release();
	m_immediateContext->Release();
	m_device->Release();

	// Release heap objects
	delete m_renderer;
	delete m_pipelineSetUp;
	delete m_d3d11SetUp;
	delete m_window;
}


// === GETTERS ===

Window* EngineHandler::GetWindow() const
{
	return m_window;
}

ID3D11Device* EngineHandler::GetDevice() const
{
	return m_device;
}

ID3D11DeviceContext* EngineHandler::GetImmediateContext() const
{
	return m_immediateContext;
}

Renderer* EngineHandler::GetRenderer() const
{
	return m_renderer;
}
