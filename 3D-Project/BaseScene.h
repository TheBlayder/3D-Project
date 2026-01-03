#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "BaseObject.h"
#include "LightHandler.h"
#include "Window.h"


namespace DX = DirectX;

// Abstract base class for scenes
class BaseScene
{
protected:
	Camera* m_camera = nullptr;
	LightHandler* m_lightHandler = nullptr;
	std::vector<BaseObject*> m_sceneObjects;

	void UpdateSceneLights(ID3D11DeviceContext* context);

	void LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window);

	virtual void LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual void LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual void LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, Window& window) = 0;

public:
	BaseScene() = default;
	virtual ~BaseScene();
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, Window& window);
	virtual void UpdateScene(const float deltaTime) = 0;

	Camera* GetCamera() const;
	LightHandler* GetLightHandler() const;
	void BindLights(ID3D11DeviceContext* context);

	void AddBaseObject(BaseObject* baseObject);
	void AddBaseObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "", const bool flipUVy = true);
	std::vector<BaseObject*>& GetBaseObjects();
};