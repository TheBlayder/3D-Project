#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Transform.h"
#include "Mesh.h"

class BaseObject {

protected:
	Transform m_transform;
	Mesh m_mesh;
	DirectX::BoundingBox m_boundingBox;

public:
	BaseObject() = default;
	BaseObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "", const bool flipUVy = true);
	virtual ~BaseObject() = default;

	virtual void Init(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "", const bool flipUVy = true);

	virtual void Update(float deltaTime) = 0;

	virtual void Draw(ID3D11DeviceContext* context) const;

	void SetBoundingBox(const DirectX::BoundingBox& box);

	Transform& GetTransform();
	Mesh& GetMesh();
	const DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::BoundingBox& GetBoundingBox();
};