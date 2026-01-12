#include "ParticleHandler.h"

void ParticleHandler::CreateShaders(ID3D11Device* device, ID3D11DeviceContext* context, std::string& vShaderByteCode)
{

	// Vertex shader
	if(!CSOReader::ReadCSO("ParticleVS.cso", vShaderByteCode))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to read particle vertex shader bytecode.");

	HRESULT hr = device->CreateVertexShader(vShaderByteCode.data(), vShaderByteCode.size(), nullptr, m_particleVS.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to create particle vertex shader.");

	std::string byteCode;
	// Pixel shader
	if(!CSOReader::ReadCSO("ParticlePS.cso", byteCode))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to read particle pixel shader bytecode.");

	hr = device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, m_particlePS.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to create particle pixel shader.");

	// Geometry shader
	if(!CSOReader::ReadCSO("ParticleGS.cso", byteCode))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to read particle geometry shader bytecode.");

	hr = device->CreateGeometryShader(byteCode.data(), byteCode.size(), nullptr, m_particleGS.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to create particle geometry shader.");

	// Compute shader
	if(!CSOReader::ReadCSO("ParticleCS.cso", byteCode))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to read particle compute shader bytecode.");

	hr = device->CreateComputeShader(byteCode.data(), byteCode.size(), nullptr, m_particleCS.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to create particle compute shader.");
}
