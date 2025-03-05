#include "BaseGameObject.h"

using namespace DirectX;

BaseGameObject::BaseGameObject(const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg)
	: m_worldScale(scale), m_worldPosition(pos), m_worldRotation(rotationInDeg), m_staticObject(false)
{
	CreateWorldMatrix(m_worldMatrix, m_worldScale, m_worldPosition, m_worldRotation);
}

void BaseGameObject::CreateWorldMatrix(XMMATRIX*& worldMatrix, const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg)
{
	XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translationMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMMATRIX rotationnMatrix = XMMatrixRotationY(DX::XMConvertToRadians(rotationInDeg));
	*worldMatrix = XMMatrixMultiplyTranspose(XMMatrixMultiply(scalingMatrix, translationMatrix), rotationnMatrix);
}

// === GETTERS ===

const XMMATRIX* BaseGameObject::GetWorldMatrix() const
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