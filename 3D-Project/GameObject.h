#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <iostream>

#include "ConstantBuffer.h"
#include "Transform.h"
#include "Mesh.h"

namespace DX = DirectX;

class GameObject
{
private:
	// World data
	Transform m_transform;

	// Texture data
	Mesh m_mesh;

public:
	GameObject() = default;
	GameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder);
	~GameObject() = default;

	void Init(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder);

	void Draw(ID3D11DeviceContext* context);

	Transform& GetTransform();
	Mesh& GetMesh();
	const DirectX::XMFLOAT4X4 GetWorldMatrix();
};
