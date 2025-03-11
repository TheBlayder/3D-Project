#include "BaseGameObject.h"

using namespace DirectX;

BaseGameObject::BaseGameObject(const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath)
	: m_worldScale(scale), m_worldPosition(pos), m_worldRotation(rotationInDeg), m_staticObject(false), m_texturePath(texturePath)
{
	CreateWorldMatrix(m_worldMatrix, m_worldScale, m_worldPosition, m_worldRotation);
	Init();
}

void BaseGameObject::CreateWorldMatrix(XMMATRIX*& worldMatrix, const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg)
{
	XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translationMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMMATRIX rotationnMatrix = XMMatrixRotationY(DX::XMConvertToRadians(rotationInDeg));
	*worldMatrix = XMMatrixMultiplyTranspose(XMMatrixMultiply(scalingMatrix, translationMatrix), rotationnMatrix);
}

// === GETTERS ===

bool BaseGameObject::IsStatic() const
{
	return m_staticObject;
}

XMMATRIX* BaseGameObject::GetWorldMatrix() const
{
	return m_worldMatrix;
}

const std::vector<DX::XMFLOAT3>& BaseGameObject::GetVertices() const
{
	return m_vertices;
}

const std::vector<DX::XMFLOAT3>& BaseGameObject::GetNormals() const
{
	return m_normals;
}

const std::vector<DX::XMFLOAT2>& BaseGameObject::GetUVs() const
{
	return m_UVs;
}