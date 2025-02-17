#include <iostream>

#include "GameObject.h"
#include "ObjectParser.h"

using namespace DirectX;

GameObject::GameObject(std::string* filePath) : staticObject(true), worldPosition(0, 0, 0), worldScale(1, 1, 1), worldRotation(0), worldMatrix(nullptr)
{
	// Load object data
	ObjectParser parser;
	parser.LoadObjectFromFile(filePath, vertices, normals, UVs);
}

GameObject::~GameObject()
{
	delete worldMatrix;
}

// === PRIVATE FUNCTIONS ===

void GameObject::CreateWorldMatrix(XMMATRIX*& worldMatrix, const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg)
{
	XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translationMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMMATRIX rotationnMatrix = XMMatrixRotationY(DX::XMConvertToRadians(rotationInDeg));
	*worldMatrix = XMMatrixMultiplyTranspose(XMMatrixMultiply(scalingMatrix, translationMatrix), rotationnMatrix);
}


// === PUBLIC FUNCTIONS ===

void GameObject::SetWorldData(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float& rotationInDeg, const bool isStatic)
{
	worldPosition = position;
	worldScale = scale;
	worldRotation = rotationInDeg;
	staticObject = isStatic;
	CreateWorldMatrix(worldMatrix, worldScale, worldPosition, worldRotation);
}

bool GameObject::IsStatic() const
{
	return staticObject;
}

void GameObject::UpdateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg)
{
	if (staticObject)
	{
		std::cerr << "Warning: Trying to update a static object!" << std::endl;
		return;
	}

	worldScale = scale;
	worldPosition = pos;
	worldRotation = rotationInDeg;
	CreateWorldMatrix(worldMatrix, worldScale, worldPosition, worldRotation);
}

const XMMATRIX* GameObject::GetWorldMatrix() const
{	
	return worldMatrix;
}
