#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

namespace DX = DirectX;

class GameObject
{
private:
	// Object data
	std::vector<DX::XMFLOAT3> vertices;
	std::vector<DX::XMFLOAT3> normals;
	std::vector<DX::XMFLOAT2> UVs;

	// World data
	DX::XMFLOAT3 worldPosition;
	DX::XMFLOAT3 worldScale;
	float worldRotation;
	DX::XMMATRIX* worldMatrix;

	bool staticObject; // If the object is static, it will not be updated every frame

	void CreateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);

public:
	GameObject(std::string* filePath);
	~GameObject();

	void SetWorldData(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float& rotationInDeg, const bool isStatic);
	bool IsStatic() const;
	void UpdateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);

	DX::XMMATRIX* GetWorldMatrix() const;
};

