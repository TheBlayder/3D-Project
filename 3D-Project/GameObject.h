#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
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
	Mesh* m_mesh;

	ConstantBuffer* m_worldBuffer;

public:
	GameObject() = default;
	GameObject(ID3D11Device*& device, const Transform& transform, Mesh* mesh);
	~GameObject() = default;

	void Init(ID3D11Device*& device, const Transform& transform, Mesh* mesh);

	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	Transform& GetTransform();
	ID3D11Buffer* GetConstantBuffer() const;
};
