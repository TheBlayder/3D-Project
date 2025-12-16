#include <DirectXMath.h>

#include "GameObject.h"
#include "HelperFuncs.h"

using namespace DirectX;
namespace MH = MatrixHelper;

GameObject::GameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "")
{
	Init(device, transform, folderPath, objectName, textureFolder);
}

void GameObject::Init(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "")
{
	m_transform = transform;

	m_mesh.Init(device, folderPath, objectName, textureFolder);
}

void GameObject::Draw(ID3D11DeviceContext* context)
{
	m_mesh.BindMeshBuffers(context);

	for (size_t i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		// Draw sub-meshes
		m_mesh.PerformSubMeshDrawCall(context, i);
	}
}

Transform& GameObject::GetTransform()
{
	return m_transform;
}

Mesh& GameObject::GetMesh()
{
	return m_mesh;
}

const DirectX::XMFLOAT4X4 GameObject::GetWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform);
	return worldMatrix;
}