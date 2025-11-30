#include "Mesh.h"

#include <DirectXMath.h>
#include <stdexcept>
#include <vector>
#include <wrl/client.h>

#include <WICTextureLoader.h>
#include "OBJ_Loader.h"
#include "SimpleVertex.h"

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

	UINT startIndex = 0;
	std::vector<unsigned int> tempIndices;
	std::vector<SimpleVertex> tempVertices;

	for(auto& mesh : loader.LoadedMeshes)
	{
		using namespace DirectX;

		// Load ambient texture
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientSRV = nullptr;
		XMFLOAT3 ambientComponent = XMFLOAT3(mesh.MeshMaterial.Ka.X, mesh.MeshMaterial.Ka.Y, mesh.MeshMaterial.Ka.Z);
		if (!mesh.MeshMaterial.map_Ka.empty())
		{
			// Load texture from file
			std::string ambientTexturePath = folderPath + mesh.MeshMaterial.map_Ka;
			std::wstring wStr(ambientTexturePath.begin(), ambientTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, ambientSRV.GetAddressOf());
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load ambient texture: " + ambientTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, ambientSRV.GetAddressOf());
		}

		// Load diffuse texture
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV = nullptr;
		XMFLOAT3 diffuseComponent = XMFLOAT3(mesh.MeshMaterial.Kd.X, mesh.MeshMaterial.Kd.Y, mesh.MeshMaterial.Kd.Z);
		if (!mesh.MeshMaterial.map_Kd.empty())
		{
			// Load texture from file
			std::string diffuseTexturePath = folderPath + mesh.MeshMaterial.map_Kd;
			std::wstring wStr(diffuseTexturePath.begin(), diffuseTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, diffuseSRV.GetAddressOf());
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load diffuse texture: " + diffuseTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, diffuseSRV.GetAddressOf());
		}

		// Load specular texture
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV = nullptr;
		XMFLOAT3 specularComponent = XMFLOAT3(mesh.MeshMaterial.Ks.X, mesh.MeshMaterial.Ks.Y, mesh.MeshMaterial.Ks.Z);
		float specularExponent = mesh.MeshMaterial.Ns == 0.0f ? 100.f : mesh.MeshMaterial.Ns; // Default exponent att 100 if none specified
		if (!mesh.MeshMaterial.map_Ks.empty())
		{
			// Load texture from file
			std::string specularTexturePath = folderPath + mesh.MeshMaterial.map_Ks;
			std::wstring wStr(specularTexturePath.begin(), specularTexturePath.end());
			HRESULT hr = CreateWICTextureFromFile(device, wStr.c_str(), nullptr, specularSRV.GetAddressOf());
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to load specular texture: " + specularTexturePath);
			}
		}
		else
		{
			CreateDefaultTexture(device, specularSRV.GetAddressOf());
		}


		// Initialize sub-mesh
		SubMesh subMesh;
		subMesh.Init(device, startIndex, mesh.Indices.size(),
			ambientSRV.Get(), diffuseSRV.Get(), specularSRV.Get(),
			ambientComponent, diffuseComponent, specularComponent, specularExponent);

		m_subMeshes.emplace_back(std::move(subMesh));

		// Append indices to tempIndices
		for(auto& index : mesh.Indices)
		{
			tempIndices.emplace_back(index + startIndex);
		}

		startIndex += mesh.Indices.size(); // Update start index for next sub-mesh

		// Converting objl::Vertex to SimpleVertex and appending to tempVertices
		for(auto& vertex : mesh.Vertices)
		{
			SimpleVertex tempVertex = SimpleVertex(vertex);
			tempVertices.emplace_back(tempVertex);
		}
	}

	// Initialize vertex buffer
	m_vertexBuffer.Init(device, tempVertices.data(), sizeof(SimpleVertex), tempVertices.size());

	// Initialize index buffer
	m_indexBuffer.Init(device, static_cast<UINT>(tempIndices.size()), tempIndices.data());

}

void Mesh::BindMeshBuffers(ID3D11DeviceContext* context) const
{
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	ID3D11Buffer* vBuffer = m_vertexBuffer.GetBuffer();
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::PerformSubMeshDrawCall(ID3D11DeviceContext* context, size_t subMeshIndex) const
{
	m_subMeshes[subMeshIndex].PerformDrawCall(context);
}

UINT Mesh::GetNrOfVerticiesInMesh() const
{
	return m_vertexBuffer.GetNrOfVertices();
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
    UINT32 defaultPixel = 0xFFFFFFFFu; // single pixel
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = 1;
    textureDesc.Height = 1;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA textureData = {};
    textureData.pSysMem = &defaultPixel;
    textureData.SysMemPitch = 4; // 1 pixel * 4 bytes (RGBA8)

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&textureDesc, &textureData, texture.GetAddressOf());
    if (FAILED(hr)) { throw std::runtime_error("Failed to create default texture."); }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureSRV);
    if (FAILED(hr)) { throw std::runtime_error("Failed to create shader resource view for default texture."); }
}