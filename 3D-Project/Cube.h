#pragma once
#include "BaseGameObject.h"

class Cube : public BaseGameObject
{
public:
	Cube(const Transform& transform, const std::string& texturePath = "");
	~Cube() = default;

private:
	void Init() override;

};

