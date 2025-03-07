#pragma once
#include "BaseGameObject.h"

class Cube : public BaseGameObject
{
public:
	Cube(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& pos, const float& rotationInDeg, const std::string& texturePath);
	~Cube() = default;

private:
	void Init() override;

};

