#pragma once
#include "BaseScene.h"
#include "ParticleHandler.h"

class TestScene : public BaseScene
{
private:
	void LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height) override;
	void LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS) override;
	void LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context) override;
	void InitializeParticles(ID3D11Device* device, ID3D11DeviceContext* context, bool hasParticles);
public:
	~TestScene() override;
	void UpdateScene(const float deltaTime, ID3D11DeviceContext* context) override;
};

