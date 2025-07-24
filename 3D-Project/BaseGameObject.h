#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <iostream>

#include "ConstantBuffer.h"
#include "Transform.h"

namespace DX = DirectX;

// BASE CLASS - Abstract class
class BaseGameObject
{
protected:
	// Object data
	std::vector<DX::XMFLOAT3> m_vertices;
	std::vector<DX::XMFLOAT3> m_normals;
	std::vector<DX::XMFLOAT2> m_UVs;

	// World data
	Transform m_transform;

	ConstantBuffer* m_worldBuffer;

	// Texture data
	std::string m_texturePath; // NOT USED YET
	unsigned char* m_textureData;
	size_t m_textureWidth;
	size_t m_textureHeight;
	size_t m_textureChannels;

	virtual void Init() = 0;

public:
	BaseGameObject(ID3D11Device*& device, const Transform& transform, const std::string& texturePath = "");
	virtual ~BaseGameObject() = default;

	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	ID3D11Buffer* GetConstantBuffer() const;
	const std::vector<DX::XMFLOAT3>& GetVertices() const;
	const std::vector<DX::XMFLOAT3>& GetNormals() const;
	const std::vector<DX::XMFLOAT2>& GetUVs() const;
};

