#include "GameObject.h"

using namespace DirectX;

void GameObject::CreateWorldMatrix(XMMATRIX*& worldMatrix, const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg)
{
	XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translationMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMMATRIX rotationnMatrix = XMMatrixRotationY(DX::XMConvertToRadians(rotationInDeg));
	*worldMatrix = XMMatrixMultiplyTranspose(XMMatrixMultiply(scalingMatrix, translationMatrix), rotationnMatrix);
}

GameObject::GameObject(std::string* filePath) : staticObject(true), worldPosition(0, 0, 0), worldScale(1, 1, 1), worldRotation(0), worldMatrix(nullptr)
{
	ObjectParser parser;
	parser.LoadObjectFromFile(filePath, vertices, normals, UVs);
}

GameObject::~GameObject()
{
	delete worldMatrix;
}

void GameObject::SetWorldData(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float& rotationInDeg, const bool isStatic)
{
	worldPosition = position;
	worldScale = scale;
	worldRotation = rotationInDeg;
	staticObject = isStatic;
	CreateWorldMatrix(worldMatrix, scale, position, rotationInDeg);
}

bool GameObject::isStatic() const
{
	return staticObject;
}

XMMATRIX* GameObject::GetWorldMatrix() const
{	
	return worldMatrix;
}
