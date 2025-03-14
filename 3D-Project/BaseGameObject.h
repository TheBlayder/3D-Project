#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <iostream>

#include "ConstantBuffer.h"

namespace DX = DirectX;

struct WorldData
{
	DX::XMFLOAT3 scale = { 1.f, 1.f, 1.f };
	DX::XMFLOAT3 position = { 0.f, 0.f, 0.f };
	float rotationY = 0.f;
};

// BASE CLASS - Abstract class
class BaseGameObject
{
protected:
	// Object data
	std::vector<DX::XMFLOAT3> m_vertices;
	std::vector<DX::XMFLOAT3> m_normals;
	std::vector<DX::XMFLOAT2> m_UVs;

	// World data
	WorldData m_worldData;

	ConstantBuffer* m_worldBuffer;

	// Texture data
	std::string m_texturePath; // NOT USED YET
	unsigned char* m_textureData;
	size_t m_textureWidth;
	size_t m_textureHeight;
	size_t m_textureChannels;

	bool m_staticObject; // If the object is static, it will not be updated every frame

	virtual void Init() = 0;

public:
	BaseGameObject(ID3D11Device*& device, const WorldData& worldData, const std::string& texturePath);
	virtual ~BaseGameObject() = default;

	bool IsStatic() const;

	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	ID3D11Buffer* GetConstantBuffer() const;
	const std::vector<DX::XMFLOAT3>& GetVertices() const;
	const std::vector<DX::XMFLOAT3>& GetNormals() const;
	const std::vector<DX::XMFLOAT2>& GetUVs() const;
};

