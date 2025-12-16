#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Player.h"
#include "Camera.h"
#include "Renderer.h"
#include "GameObject.h"

namespace DX = DirectX;

// Abstract base class for scenes
class BaseScene
{
protected:
	Camera* m_camera;
	std::vector<GameObject*> m_gameObjects;

public:
	BaseScene();
	virtual ~BaseScene() {};
	bool Init(ID3D11Device* device, ID3D11DeviceContext* context);
	virtual void UpdateScene() = 0;

	Camera* GetCamera() const;

	void AddGameObject(GameObject* gameObject);
	void AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder);
	std::vector<GameObject*>& GetGameObjects();
};