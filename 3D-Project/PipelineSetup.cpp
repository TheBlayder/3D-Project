#include "PipelineSetUp.h"

PipelineSetUp::PipelineSetUp()
{
}

PipelineSetUp::~PipelineSetUp()
{
}

bool PipelineSetUp::SetUp(ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, ID3D11Buffer*& vertexBuffer, 
	ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, ID3D11InputLayout*& inputLayout, 
	ID3D11Buffer*& VScBuffer, ID3D11Buffer*& PScBuffer, 
	ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureView, ID3D11SamplerState*& samplerState, unsigned char*& textureData)
{
	return false;
}

bool PipelineSetUp::LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
	return false;
}

bool PipelineSetUp::CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	return false;
}

bool PipelineSetUp::CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	return false;
}

bool PipelineSetUp::CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& SRVTexture, unsigned char*& textureData)
{
	return false;
}

bool PipelineSetUp::CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& samplerState)
{
	return false;
}
