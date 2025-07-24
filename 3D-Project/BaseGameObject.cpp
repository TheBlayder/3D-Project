#include "BaseGameObject.h"
#include "HelperFuncs.h"

using namespace DirectX;
namespace MH = MatrixHelper;

BaseGameObject::BaseGameObject(ID3D11Device*& device, const Transform& transform, const std::string& texturePath)
	: m_staticObject(false), m_transform(transform), m_texturePath(texturePath)
{
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform.GetScaleF3(), m_transform.GetPositionF3(), m_transform.GetRotationF3());
	m_worldBuffer = new ConstantBuffer(device, sizeof(XMFLOAT4X4), &worldMatrix);

	Init();
}

// === GETTERS ===

void BaseGameObject::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	XMFLOAT4X4 worldMatrix;
	MH::CreateWorldMatrix(worldMatrix, m_transform.GetScaleF3(), m_transform.GetPositionF3(), m_transform.GetRotationF3());
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