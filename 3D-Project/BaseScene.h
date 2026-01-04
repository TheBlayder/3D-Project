#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "LightHandler.h"
#include "Window.h"

#include "BaseObject.h"
#include "DCEM.h"

namespace DX = DirectX;

// Abstract base class for scenes
class BaseScene
{
protected:
	Camera* m_camera = nullptr;
	LightHandler* m_lightHandler = nullptr;
	std::vector<BaseObject*> m_sceneObjects;
	std::vector<DCEM*> m_dcemObjects;

	void UpdateSceneLights(ID3D11DeviceContext* context);

	void LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height);

	virtual void LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual void LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual void LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height) = 0;

public:
	BaseScene() = default;
	virtual ~BaseScene();
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height);
	virtual void UpdateScene(const float deltaTime) = 0;

	Camera* GetCamera() const;
	LightHandler* GetLightHandler() const;
	void BindLights(ID3D11DeviceContext* context);

	void AddBaseObject(BaseObject* baseObject);
	void AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, 
		const std::string& textureFolder = "", const bool flipUVy = true);
	void AddDCEMObject(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName);
	std::vector<BaseObject*>& GetSceneObjects();
	std::vector<DCEM*>& GetDCEMObjects();
};