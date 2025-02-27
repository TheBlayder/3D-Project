#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "EngineHandler.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

private:
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

public:
	void RenderFrame(
		ID3D11DeviceContext*& immediateContext, 
		ID3D11RenderTargetView*& rtv,
		ID3D11DepthStencilView*& dsView, 
		D3D11_VIEWPORT& viewport, 
		ID3D11VertexShader*& vShader,
		ID3D11PixelShader*& pShader, 
		ID3D11InputLayout*& inputLayout,
		ID3D11Buffer*& vertexBuffer, 
		ID3D11ShaderResourceView*& SRVTexture, 
		ID3D11SamplerState*& samplerState,
		const UINT& stride,
		const UINT& offset,
		const UINT& nrOfVertices);

};