#include "Cube.h"

Cube::Cube(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath)
	: BaseGameObject(scale, pos, rotationInDeg, texturePath)
{
	Init();
}

void Cube::Init()
{
	m_vertices = 
	{
		// Front face
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		// Back face
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f }
	};
	
	m_normals = {
		// Front face
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		// Back face
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	
	m_UVs =
	{
		// Front face
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		// Back face
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		{ 0.0f, 0.0f }
	};
}
