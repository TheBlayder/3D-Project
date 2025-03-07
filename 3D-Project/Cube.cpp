#include "Cube.h"

Cube::Cube(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath)
	: BaseGameObject(scale, pos, rotationInDeg, texturePath)
{
	Init();
}

void Cube::Init()
{
	
}
