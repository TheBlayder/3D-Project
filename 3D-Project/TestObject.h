#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include "Transform.h"
#include "SimpleVertex.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"

/// <summary>
/// Class used for testing purposes.
/// </summary>
class TestObject
{
private:
	Transform m_transform;
	std::vector<SimpleVertex> m_vertices;

	VertexBuffer m_vertexBuffer;
public:
	// Change the constructor to accept a pointer, not a reference to pointer
	TestObject(ID3D11Device* device);
	~TestObject() = default;

	DirectX::XMFLOAT4X4 GetWorldMatrix() const;

	void Draw(ID3D11DeviceContext* context);
};