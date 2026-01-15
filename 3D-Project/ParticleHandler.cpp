#include "ParticleHandler.h"

#include <cstdlib>

void ParticleHandler::CreateShaders(ID3D11Device* device, ID3D11DeviceContext* context)
{

	std::string byteCode;
	// Vertex shader
	if(!CSOReader::ReadCSO("ParticleVS.cso", byteCode))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to read particle vertex shader bytecode.");

	HRESULT hr = device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, m_particleVS.GetAddressOf());
	if(FAILED(hr))
		throw std::runtime_error("ParticleHandler::CreateShaders - failed to create particle vertex shader.");

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

void ParticleHandler::Init(ID3D11Device* device, ID3D11DeviceContext* context, UINT nrOfParticles, bool dynamic, bool hasSRV, bool hasUAV)
{
	this->CreateShaders(device, context);

	TimeBufferData data = {};
	m_timeBuffer.Init(device, sizeof(TimeBufferData), &data);

	CameraBufferData camData = {};
	DX::XMMATRIX identity = DX::XMMatrixIdentity();
	DX::XMStoreFloat4x4(&camData.vpMatrix, identity);
	m_cameraBuffer.Init(device, sizeof(CameraBufferData), &camData);

	// Create init data for particles
	std::vector<Particle> initData(nrOfParticles);
	
	for (size_t i = 0; i < nrOfParticles; ++i)
	{
		initData[i].position = { 0.f, -2.f, 0.f };
		initData[i].velocity = { 0.f, 0.f, 0.f };
		initData[i].color = { 0.f, 0.f, 1.f, 1.f };
		initData[i].size = static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * 0.5f; // Generate random number [0, 0.5] for smaller particles
	}

	m_particleBuffer.Init(device, sizeof(Particle), nrOfParticles, dynamic, hasSRV, hasUAV, initData.data());
}

void ParticleHandler::Update(ID3D11DeviceContext* context, const float deltaTime)
{
	// Update Constant Buffers
	TimeBufferData timeData = {};
	timeData.deltaTime = deltaTime;
	m_timeBuffer.Update(context, &timeData);

	// Update particles with Compute Shader
	context->CSSetShader(m_particleCS.Get(), nullptr, 0);
	context->CSSetConstantBuffers(1, 1, m_timeBuffer.GetBufferPtr());
	context->CSSetUnorderedAccessViews(1, 1, m_particleBuffer.GetUAVPtr(), nullptr);

	UINT threadGroupSize = std::ceil(m_particleBuffer.GetNrOfParticles() / 32.f);
	context->Dispatch(threadGroupSize, 1, 1);

	ID3D11Buffer* nullBuffer = nullptr;
	context->CSSetConstantBuffers(1, 1, &nullBuffer);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	context->CSSetUnorderedAccessViews(1, 1, &nullUAV, nullptr);
}

void ParticleHandler::Draw(ID3D11DeviceContext* context, Camera* camera, D3D_PRIMITIVE_TOPOLOGY returnTopology, ID3D11InputLayout* returnInputLayout)
{
	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->VSSetShader(m_particleVS.Get(), nullptr, 0);
	context->VSSetShaderResources(0, 1, m_particleBuffer.GetSRVPtr());

	CameraBufferData camData = {};
	camData.vpMatrix = camera->GetViewProjMatrix();
	DX::XMVECTOR camPos = camera->GetPosition();
	DX::XMStoreFloat3(&camData.cameraPosition, camPos);
	m_cameraBuffer.Update(context, &camData);
	context->GSSetShader(m_particleGS.Get(), nullptr, 0);
	context->GSSetConstantBuffers(0, 1, m_cameraBuffer.GetBufferPtr());

	context->PSSetShader(m_particlePS.Get(), nullptr, 0);

	context->Draw(m_particleBuffer.GetNrOfParticles(), 0);

	// Restore previous state
	context->IASetPrimitiveTopology(returnTopology);
	context->IASetInputLayout(returnInputLayout);
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	context->VSSetShaderResources(0, 1, &nullSRV);
}
