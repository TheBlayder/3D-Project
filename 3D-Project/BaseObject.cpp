#include "BaseObject.h"

#include "HelperFuncs.h"

BaseObject::BaseObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, 
	const std::string& textureFolder, const bool tesselate, const bool flipUVy)
{
	Init(device, transform, folderPath, objectName, textureFolder, tesselate, flipUVy);
}

void BaseObject::Init(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, 
	const std::string& textureFolder, const bool tesselate, const bool flipUVy)
{
	m_transform = transform;
	m_tesselationEnabled = tesselate;
	m_mesh.Init(device, folderPath, objectName, textureFolder, flipUVy);
	
	DirectX::BoundingBox localBoundingBox = m_mesh.GetBoundingBox();
	DirectX::XMFLOAT4X4 worldMatrixF4 = GetWorldMatrix();
	
	// Transpose back to get the correct matrix for DirectXCollision functions
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&worldMatrixF4));
	
	localBoundingBox.Transform(m_boundingBox, worldMatrix);
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

void BaseObject::SetBoundingBox(const DirectX::BoundingBox& box)
{
	m_boundingBox = box;
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

DirectX::BoundingBox& BaseObject::GetBoundingBox()
{
	return m_boundingBox;
}
