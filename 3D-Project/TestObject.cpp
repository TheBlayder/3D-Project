#include <DirectXMath.h>

#include "TestObject.h"
#include "HelperFuncs.h"

TestObject::TestObject(ID3D11Device* device)
{
	using namespace DirectX;

	m_transform.SetPosition(XMVectorSet(0.0f, 0.0f, 20.0f, 0.0f));
	m_transform.SetRotation(XMVectorSet(0.0f, 0.0001f, 0.0f, 0.0f));
	m_transform.SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));

	// Set up vertices for a simple triangle using SimpleVertex structure
	m_vertices = {
		SimpleVertex{ { 1.0f, 1.0f, 0.0f  }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		SimpleVertex{ { 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		SimpleVertex{ { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } }
	};

	m_vertexBuffer.Init(device, m_vertices.data(), sizeof(SimpleVertex), m_vertices.size());
}

DirectX::XMFLOAT4X4 TestObject::GetWorldMatrix() const
{
	DirectX::XMFLOAT4X4 worldMatrix;
	MatrixHelper::CreateWorldMatrix(worldMatrix, m_transform);

	return worldMatrix;
}

void TestObject::Draw(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = m_vertexBuffer.GetBuffer();
	if (!vertexBuffer) return;

	// Bind vertex buffer and set triangle list topology, then draw
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->Draw(static_cast<UINT>(m_vertexBuffer.GetNrOfVertices()), 0);
}
