#include <iostream>
#include <fstream>
#include <string>

#include "PipelineSetUp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
	{
		std::cerr << "Error loading shaders!" << std::endl;
		return false;
	}

	if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	if (!CreateVertexBuffer(device, vertexBuffer))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}

	/*if (!CreateVSConstantBuffers(device, immediateContext, vertexShaderConstantBuffer))
	{
		std::cerr << "Error creating consant buffers!" << std::endl;
		return false;
	}

	if (!CreatePSConstantBuffers(device, immediateContext, PSPointLightConstantBuffer, PSMaterialConstantBuffer))
	{
		std::cerr << "Error creating consant buffers!" << std::endl;
		return false;
	}*/

	if (!(CreateTexture(device, texture, textureView, textureData)))
	{
		std::cerr << "Error creating texture!" << std::endl;
		return false;
	}

	if (!(CreateSamplerState(device, samplerState)))
	{
		std::cerr << "Error creating sampler state!" << std::endl;
		return false;
	}
}

bool PipelineSetUp::LoadShaders(ID3D11Device*& device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("VertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();
	reader.open("PixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;
}

bool PipelineSetUp::CreateInputLayout(ID3D11Device*& device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool PipelineSetUp::CreateVertexBuffer(ID3D11Device*& device, ID3D11Buffer*& vertexBuffer, BaseGameObject*& object)
{
	return true;
}

bool PipelineSetUp::CreateTexture(ID3D11Device*& device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& SRVTexture, unsigned char*& textureData)
{
	int width, height, channels;

	textureData = stbi_load("Textures/ProfilePic.png", &width, &height, &channels, 4);

	if (!textureData)
	{
		std::cerr << "Failed to load texture!" << std::endl;
		return false;
	}

	if (channels != 4)
	{
		std::cerr << "Image does not have 4 channels!" << std::endl;
		stbi_image_free(textureData);
		return false;
	}

	D3D11_TEXTURE2D_DESC texture2DDesc;
	texture2DDesc.Width = width;
	texture2DDesc.Height = height;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2DDesc.CPUAccessFlags = 0;
	texture2DDesc.MiscFlags = 0;
	texture2DDesc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA texture2DSubData;
	texture2DSubData.pSysMem = textureData;
	texture2DSubData.SysMemPitch = width * channels;
	texture2DSubData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&texture2DDesc, &texture2DSubData, &texture);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create 2Dtexture!" << std::endl;

		stbi_image_free(textureData);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texture2DDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(texture, &srvDesc, &SRVTexture);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create shader resource view!" << std::endl;

		stbi_image_free(textureData);
		return false;
	}
}

bool PipelineSetUp::CreateSamplerState(ID3D11Device*& device, ID3D11SamplerState*& samplerState)
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

	HRESULT hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	return !FAILED(hr);
}
