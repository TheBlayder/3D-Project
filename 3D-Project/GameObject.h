#pragma once

#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "ObjectParser.h"

namespace DX = DirectX;
class GameObject
{
private:
	std::vector<DX::XMFLOAT3> vertices;
	std::vector<DX::XMFLOAT3> normals;
	std::vector<DX::XMFLOAT2> UVs;

	DX::XMFLOAT3 worldPosition;
	DX::XMFLOAT3 worldScale;
	float worldRotation;
	DX::XMMATRIX* worldMatrix;

	bool staticObject;

	void CreateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg);

public:
	GameObject(std::string* filePath);
	~GameObject();

	void SetWorldData(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float& rotationInDeg, const bool isStatic);
	bool isStatic() const;

	DX::XMMATRIX* GetWorldMatrix() const;
};

