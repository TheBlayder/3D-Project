#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

class PipelineSetUp
{
public:
	PipelineSetUp();
	~PipelineSetUp();

	bool SetUp(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader,
		ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& VScBuffer, ID3D11Buffer*& PScBuffer,
		ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureView, ID3D11SamplerState*& samplerState, unsigned char*& textureData);

private:
	bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode);
	bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode);
	bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer);
	bool CreatePScBuffer();
	bool CreateVScBuffer();
	bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& SRVTexture, unsigned char*& textureData);
	bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState);
};