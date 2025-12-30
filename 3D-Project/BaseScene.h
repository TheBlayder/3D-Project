#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "GameObject.h"
#include "LightHandler.h"
#include "Window.h"


namespace DX = DirectX;

// Abstract base class for scenes
class BaseScene
{
protected:
	Camera* m_camera;
	LightHandler* m_lightHandler;
	std::vector<GameObject*> m_gameObjects;

	void UpdateSceneLights(ID3D11DeviceContext* context);

	virtual bool LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window) = 0;

public:
	BaseScene() = default;
	virtual ~BaseScene() {};
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, Window& window);
	virtual void UpdateScene(const float deltaTime) = 0;

	Camera* GetCamera() const;
	void BindLights(ID3D11DeviceContext* context);

	void AddGameObject(GameObject* gameObject);
	void AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "", const bool flipUVy = false);
	std::vector<GameObject*>& GetGameObjects();
};