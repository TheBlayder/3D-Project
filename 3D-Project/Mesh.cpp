#include "Mesh.h"

#include <DirectXMath.h>
#include <stdexcept>
#include <vector>

#include "WICTextureLoader.h"

#include "OBJ_Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Mesh::Mesh(ID3D11Device* device, const std::string& folderPath, const std::string& objectName)
{
	Init(device, folderPath, objectName);
}

void Mesh::Init(ID3D11Device* device, const std::string& folderPath, const std::string& objectName)
{
	const std::string filePath = folderPath + "/" + objectName;
	objl::Loader loader;
	if (!loader.LoadFile(filePath))
	{
		throw std::runtime_error("Failed to load mesh from file: " + filePath);
		return;
	}

	size_t startIndex = 0;
	std::vector<unsigned int> tempIndices;

	for(auto& mesh : loader.LoadedMeshes)
	{
		SubMesh subMesh;
		ID3D11ShaderResourceView* ambientSRV = nullptr;
		ID3D11ShaderResourceView* diffuseSRV = nullptr;
		ID3D11ShaderResourceView* specularSRV = nullptr;

		using namespace DirectX;

		// Load ambient texture
		XMFLOAT3 ambientComponent = XMFLOAT3(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
		if (!mesh.MeshMaterial.map_Ka.empty())
		{
			// Load texture from file
			std::string ambientTexturePath = folderPath + mesh.MeshMaterial.map_Ka;
			std::wstring wStr(ambientTexturePath.begin(), ambientTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &ambientSRV);
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
		XMFLOAT3 diffuseComponent = XMFLOAT3(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
		if (!mesh.MeshMaterial.map_Kd.empty())
		{
			// Load texture from file
			std::string diffuseTexturePath = folderPath + mesh.MeshMaterial.map_Kd;
			std::wstring wStr(diffuseTexturePath.begin(), diffuseTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &diffuseSRV);
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
		XMFLOAT3 specularComponent = XMFLOAT3(mesh.MeshMaterial.Ks.X, mesh.MeshMaterial.Ks.Y, mesh.MeshMaterial.Ks.Z);
		float specularExponent = mesh.MeshMaterial.Ns == 0.0f ? 100.f : mesh.MeshMaterial.Ns; // Default exponent att 100 if none specified
		if (!mesh.MeshMaterial.map_Ks.empty())
		{
			// Load texture from file
			std::string specularTexturePath = folderPath + mesh.MeshMaterial.map_Ks;
			std::wstring wStr(specularTexturePath.begin(), specularTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, &specularSRV);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load specular texture: " + specularTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, &specularSRV);
		}

		// Initialize sub-mesh
		subMesh.Init(device, startIndex, mesh.Indices.size(),
			ambientSRV, diffuseSRV,  specularSRV,
			ambientComponent, diffuseComponent, specularComponent, specularExponent);

		m_subMeshes.push_back(std::move(subMesh));

		// Append indices to tempIndices
		for(auto& index : mesh.Indices)
		{
			tempIndices.push_back(index + startIndex);
		}

		startIndex += mesh.Indices.size(); // Update start index for next sub-mesh
	}

	// Initialize vertex buffer
	m_vertexBuffer.Init(device, loader.LoadedVertices.data(), sizeof(objl::Vertex), loader.LoadedVertices.size());

	// Initialize index buffer
	m_indexBuffer.Init(device, static_cast<UINT>(tempIndices.size()), tempIndices.data());
}

void Mesh::BindMeshBuffers(ID3D11DeviceContext* context) const
{
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetBufferPtr(), &stride, &offset);

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