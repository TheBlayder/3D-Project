#include <iostream>

#include "GameObject.h"
#include "ObjectParser.h"

using namespace DirectX;

GameObject::GameObject(std::string* filePath) : m_staticObject(true), m_worldPosition(0, 0, 0), m_worldScale(1, 1, 1), m_worldRotation(0), m_worldMatrix(nullptr)
{
	// Load object data
	ObjectParser parser;
	parser.LoadObjectFromFile(filePath, m_vertices, m_normals, m_UVs);
}

GameObject::~GameObject()
{
	delete m_worldMatrix;
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
	m_worldPosition = position;
	m_worldScale = scale;
	m_worldRotation = rotationInDeg;
	m_staticObject = isStatic;
	CreateWorldMatrix(m_worldMatrix, m_worldScale, m_worldPosition, m_worldRotation);
}

bool GameObject::IsStatic() const
{
	return m_staticObject;
}

void GameObject::UpdateWorldMatrix(DX::XMMATRIX*& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg)
{
	if (m_staticObject)
	{
		std::cerr << "Warning: Trying to update a static object!" << std::endl;
		return;
	}

	m_worldScale = scale;
	m_worldPosition = pos;
	m_worldRotation = rotationInDeg;
	CreateWorldMatrix(worldMatrix, m_worldScale, m_worldPosition, m_worldRotation);
}


// === GETTERS ===

const XMMATRIX* GameObject::GetWorldMatrix() const
{	
	return m_worldMatrix;
}

const std::vector<DX::XMFLOAT3>& GameObject::GetVertices() const
{
	return m_vertices;
}

const std::vector<DX::XMFLOAT3>& GameObject::GetNormals() const
{
	return m_normals;
}

const std::vector<DX::XMFLOAT2>& GameObject::GetUVs() const
{
	return m_UVs;
}
