#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "Renderer.h"
#include "GameObject.h"

namespace DX = DirectX;

class BaseScene
{
private:
	Camera* m_camera;
	std::vector<GameObject*> m_gameObjects;

public:
	BaseScene();
	~BaseScene();

	Camera* GetCamera() const;
	void SetCamera(Camera* camera);
	
	void AddGameObject(GameObject* gameObject);
	void AddGameObject(std::string& filepath);
	std::vector<GameObject*> GetGameObjects() const;
};