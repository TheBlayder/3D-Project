#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

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

	bool m_staticObject; // If the object is static, it will not be updated every frame
	void CreateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);

	virtual void Init() = 0;

public:
	BaseGameObject(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);
	virtual ~BaseGameObject() = default;

	const DX::XMMATRIX* GetWorldMatrix() const;
	const std::vector<DX::XMFLOAT3>& GetVertices() const;
	const std::vector<DX::XMFLOAT3>& GetNormals() const;
	const std::vector<DX::XMFLOAT2>& GetUVs() const;
};

