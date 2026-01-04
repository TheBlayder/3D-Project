#pragma once
#include "BaseScene.h"
class TestScene : public BaseScene
{
private:
	void LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height) override;
	void LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context) override;
public:
	~TestScene() = default;
	void UpdateScene(const float deltaTime) override;
};

