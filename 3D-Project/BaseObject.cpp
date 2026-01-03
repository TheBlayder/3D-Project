#include "BaseObject.h"

#include "HelperFuncs.h"

BaseObject::BaseObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder, const bool flipUVy)
{
	Init(device, transform, folderPath, objectName, textureFolder, flipUVy);
}

void BaseObject::Init(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder, const bool flipUVy)
{
	m_transform = transform;
	m_mesh.Init(device, folderPath, objectName, textureFolder, flipUVy);
}

void BaseObject::Draw(ID3D11DeviceContext* context) const
{
	m_mesh.BindMeshBuffers(context);

	for (size_t i = 0; i < m_mesh.GetNrOfSubMeshes(); ++i)
	{
		// Draw sub-meshes
		m_mesh.PerformSubMeshDrawCall(context, i);
	}
}

Transform& BaseObject::GetTransform()
{
	return m_transform;
}

Mesh& BaseObject::GetMesh()
{
	return m_mesh;
}

const DirectX::XMFLOAT4X4 BaseObject::GetWorldMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 worldMatrix;
	MatrixHelper::CreateWorldMatrix(worldMatrix, m_transform);
	return worldMatrix;
}