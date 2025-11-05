#include "GameObject.h"
#include "HelperFuncs.h"

using namespace DirectX;
namespace MH = MatrixHelper;

GameObject::GameObject(ID3D11Device*& device, const Transform& transform, Mesh* mesh)
{
	Init(device, transform, mesh);
}

void GameObject::Init(ID3D11Device*& device, const Transform& transform, Mesh* mesh)
{
	m_transform = transform;

	m_mesh = mesh;
	
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

Transform& GameObject::GetTransform()
{
	return m_transform;
}

// === GETTERS ===
ID3D11Buffer* GameObject::GetConstantBuffer() const
{
	return m_worldBuffer->GetBuffer();
}