#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::RenderFrame(ID3D11DeviceContext*& immediateContext, ID3D11RenderTargetView*& rtv, ID3D11DepthStencilView*& dsView, 
	D3D11_VIEWPORT& viewport, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, 
	ID3D11Buffer*& vertexBuffer, ID3D11ShaderResourceView*& SRVTexture, ID3D11SamplerState*& samplerState, 
	const UINT& stride, const UINT& offset, const UINT& nrOfVertices)
{
	immediateContext->ClearRenderTargetView(rtv, this->clearColor);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->VSSetShader(vShader, nullptr, 0);
	immediateContext->PSSetShader(pShader, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, &SRVTexture);
	immediateContext->PSSetSamplers(0, 1, &samplerState);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);

	immediateContext->Draw(nrOfVertices, 0);
}
