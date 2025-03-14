#include "BaseGameObject.h"
#include "MathFuncs.h"

using namespace DirectX;

BaseGameObject::BaseGameObject(ID3D11Device*& device, const XMFLOAT3& scale, const XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath)
	: m_staticObject(false), m_texturePath(texturePath)
{
	m_worldData.scale = scale;
	m_worldData.position = pos;
	m_worldData.rotationY = rotationInDeg;
	
	m_worldBuffer = new ConstantBuffer(device, sizeof(DX::XMFLOAT4X4), &CreateWorldMatrix(m_worldData.scale, m_worldData.position, m_worldData.rotationY));

	Init();
}

// === GETTERS ===

bool BaseGameObject::IsStatic() const
{
	return m_staticObject;
}

void BaseGameObject::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	m_worldBuffer->Update(context, &CreateWorldMatrix(m_worldData.scale, m_worldData.position, m_worldData.rotationY));
}

ID3D11Buffer* BaseGameObject::GetConstantBuffer() const
{
	return m_worldBuffer->GetBuffer();
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