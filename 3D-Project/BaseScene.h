#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "Camera.h"
#include "LightHandler.h"
#include "Window.h"

#include "BaseObject.h"
#include "DCEM.h"
#include "QuadTree.h"
#include "ParticleHandler.h"

namespace DX = DirectX;

// Abstract base class for scenes
class BaseScene
{
protected:
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<LightHandler> m_lightHandler;
	std::vector<std::unique_ptr<BaseObject>> m_sceneObjects;
	std::vector<std::unique_ptr<DCEM>> m_dcemObjects;

	QuadTree<BaseObject> m_quadTree;

	Window* window = nullptr;

	bool m_hasParticles = false;
	ParticleHandler m_particleHandler;

	void UpdateSceneLights(ID3D11DeviceContext* context);

	void LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height, 
		ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, bool hasParticles);

	virtual void LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual void LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS) = 0;
	virtual void LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height) = 0;
	virtual void InitializeParticles(ID3D11Device* device, ID3D11DeviceContext* context, bool hasParticles) = 0;

public:
	BaseScene() = default;
	virtual ~BaseScene();
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, const Window* window, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, bool hasParticles);
	virtual void UpdateScene(const float deltaTime, ID3D11DeviceContext* context, ConstantBuffer* worldBuffer, ConstantBuffer* viewProjBuffer) = 0;


	void AddBaseObject(std::unique_ptr<BaseObject> baseObject);
	void AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, 
		const std::string& textureFolder = "", const bool tesselate = false, const bool flipUVy = true);
	void AddDCEMObject(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName,
		ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS);

	Camera* GetCamera() const;
	LightHandler* GetLightHandler() const;
	void BindLights(ID3D11DeviceContext* context);
	Window* GetWindow() const { return window; }

	bool HasParticles() const { return m_hasParticles; }
	ParticleHandler& GetParticleHandler() { return m_particleHandler; }

	std::vector<std::unique_ptr<BaseObject>>& GetSceneObjects();
	std::vector<BaseObject*> GetVisableSceneObjects(Camera* camera);
	std::vector<std::unique_ptr<DCEM>>& GetDCEMObjects();
};