#include "GameObject.h"
#include "HelperFuncs.h"

using namespace DirectX;
namespace MH = MatrixHelper;

GameObject::GameObject(ID3D11Device*& device, const Transform& transform, std::string& folderPath, std::string& objectName)
{
	Init(device, transform, folderPath, objectName);
}

void GameObject::Init(ID3D11Device*& device, const Transform& transform, std::string& folderPath, std::string& objectName)
{
	m_transform = transform;

	m_mesh->Init(device, folderPath, objectName);
	
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform);
	m_worldBuffer = new ConstantBuffer(device, sizeof(XMFLOAT4X4), &worldMatrix);
}

void GameObject::Draw(ID3D11DeviceContext* context)
{
	m_mesh->BindMeshBuffers(context);

	for (size_t i = 0; i < m_mesh->GetNrOfSubMeshes(); ++i)
	{
		// Draw sub-meshes
		m_mesh->PerformSubMeshDrawCall(context, i);
	}
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

Mesh* GameObject::GetMesh()
{
	return m_mesh;
}

// === GETTERS ===
ID3D11Buffer* GameObject::GetConstantBuffer() const
{
	return m_worldBuffer->GetBuffer();
}