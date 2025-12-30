#pragma once
#include "BaseScene.h"
class TestScene : public BaseScene
{
private:
	bool LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window) override;
public:
	~TestScene() = default;
	void UpdateScene(const float deltaTime) override;
};

