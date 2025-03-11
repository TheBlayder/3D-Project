#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <iostream>

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
	DX::XMFLOAT3 m_worldScale;
	DX::XMFLOAT3 m_worldPosition;
	float m_worldRotation;
	DX::XMMATRIX* m_worldMatrix;

	// Texture data
	std::string m_texturePath; // NOT USED YET
	unsigned char* m_textureData;
	size_t m_textureWidth;
	size_t m_textureHeight;
	size_t m_textureChannels;

	bool m_staticObject; // If the object is static, it will not be updated every frame
	void CreateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);

	virtual void Init() = 0;

public:
	BaseGameObject(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath);
	virtual ~BaseGameObject() = default;

	bool IsStatic() const;
	DX::XMMATRIX* GetWorldMatrix() const;
	const std::vector<DX::XMFLOAT3>& GetVertices() const;
	const std::vector<DX::XMFLOAT3>& GetNormals() const;
	const std::vector<DX::XMFLOAT2>& GetUVs() const;
};

