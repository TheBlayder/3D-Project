#include "Mesh.h"

#include <DirectXMath.h>
#include <stdexcept>
#include <vector>

#include "WICTextureLoader.h"

#include "OBJ_Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Mesh::Mesh(ID3D11Device* device, const std::string& filePath)
{
	Init(device, filePath);
}

void Mesh::Init(ID3D11Device* device, const std::string& filePath)
{
	objl::Loader loader;
	bool isLoaded = loader.LoadFile(filePath);
	if (!isLoaded)
	{
		throw std::runtime_error("Failed to load mesh from file: " + filePath);
		return;
	}

	size_t startIndex = 0;
	std::vector<unsigned int> tempIndices;
	std::vector<objl::Vertex> tempVertices;

	for(auto& mesh : loader.LoadedMeshes)
	{
		SubMesh subMesh;
		ID3D11ShaderResourceView* ambientSRV = nullptr;
		ID3D11ShaderResourceView* diffuseSRV = nullptr;
		ID3D11ShaderResourceView* specularSRV = nullptr;

		// Load ambient texture
		if (!mesh.MeshMaterial.map_Ka.empty())
		{
			// Load texture from file
			std::string ambientTexturePath = mesh.MeshMaterial.map_Ka;
			std::wstring wStr;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &ambientSRV);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load ambient texture: " + ambientTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, &ambientSRV);
		}

		// Load diffuse texture
		if (!mesh.MeshMaterial.map_Kd.empty())
		{
			// Load texture from file
			std::string diffuseTexturePath = mesh.MeshMaterial.map_Kd;
			std::wstring wStr;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &diffuseSRV);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load diffuse texture: " + diffuseTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, &diffuseSRV);
		}

		// Load specular texture
		if (!mesh.MeshMaterial.map_Ks.empty())
		{
			// Load texture from file
			std::string specularTexturePath = mesh.MeshMaterial.map_Ks;
			std::wstring wStr;
			HRESULT hr = DirectX::CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &specularSRV);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load specular texture: " + specularTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, &specularSRV);
		}

		
	}

	
}

void Mesh::BindMeshBuffers(ID3D11DeviceContext* context) const
{
	ID3D11Buffer* vBuffer = m_vertexBuffer.GetBuffer();
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const
{
	m_subMeshes[subMeshIndex].PerformDrawCall(context);
}

size_t Mesh::GetNrOfSubMeshes() const
{
	return m_subMeshes.size();
}

ID3D11ShaderResourceView* Mesh::GetAmbientSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetAmbientSRV();
}

ID3D11ShaderResourceView* Mesh::GetDiffuseSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetDiffuseSRV();
}

ID3D11ShaderResourceView* Mesh::GetSpecularSRV(size_t subMeshIndex) const
{
	return m_subMeshes[subMeshIndex].GetSpecularSRV();
}

void Mesh::CreateDefaultTexture(ID3D11Device* device, ID3D11ShaderResourceView** textureSRV)
{
	unsigned char defaultTextureData[4] = { 255, 255, 255, 255 }; // White pixel
	
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = 1;
	textureDesc.Height = 1;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	
	D3D11_SUBRESOURCE_DATA textureData = {};
	textureData.pSysMem = defaultTextureData;
	textureData.SysMemPitch = sizeof(defaultTextureData);
	
	ID3D11Texture2D* texture = nullptr;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &textureData, &texture);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create default texture.");
	
	}
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	
	hr = device->CreateShaderResourceView(texture, &srvDesc, textureSRV);
	texture->Release();
	
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create shader resource view for default texture.");
	}
}