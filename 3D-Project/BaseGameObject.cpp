#include "BaseGameObject.h"
#include "MathFuncs.h"

using namespace DirectX;

BaseGameObject::BaseGameObject(ID3D11Device*& device, const WorldData& worldData, const std::string& texturePath)
	: m_staticObject(false), m_worldData(worldData), m_texturePath(texturePath)
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	CreateWorldMatrix(worldMatrix, m_worldData.scale, m_worldData.position, m_worldData.rotationY);
	m_worldBuffer = new ConstantBuffer(device, sizeof(DX::XMFLOAT4X4), &worldMatrix);

	Init();
}

// === GETTERS ===

bool BaseGameObject::IsStatic() const
{
	return m_staticObject;
}

void BaseGameObject::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	CreateWorldMatrix(worldMatrix, m_worldData.scale, m_worldData.position, m_worldData.rotationY);
	m_worldBuffer->Update(context, &worldMatrix);
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