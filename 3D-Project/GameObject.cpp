#include "GameObject.h"
#include "HelperFuncs.h"

using namespace DirectX;
namespace MH = MatrixHelper;

GameObject::GameObject(ID3D11Device*& device, const Transform& transform, const std::string& texturePath)
	: m_transform(transform), m_texturePath(texturePath)
{
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform);
	m_worldBuffer = new ConstantBuffer(device, sizeof(XMFLOAT4X4), &worldMatrix);
}

void GameObject::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform);
	m_worldBuffer->Update(context, &worldMatrix);
}

// === GETTERS ===
ID3D11Buffer* GameObject::GetConstantBuffer() const
{
	return m_worldBuffer->GetBuffer();
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