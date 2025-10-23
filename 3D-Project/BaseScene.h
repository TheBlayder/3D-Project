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

class BaseScene
{
private:
	Camera* m_camera;
	std::vector<GameObject*> m_gameObjects;

public:
	BaseScene();
	~BaseScene();
	bool Init();
	virtual void UpdateScene() = 0;

	Camera* GetCamera() const;

	void AddGameObject(GameObject* gameObject);
	std::vector<GameObject*> GetGameObjects() const;
};