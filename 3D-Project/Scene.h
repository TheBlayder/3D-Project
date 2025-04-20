#pragma once

#include <vector>
#include <string>
#include <DirectXMath.h>

#include "Camera.h"
#include "Renderer.h"
#include "BaseGameObject.h"

namespace DX = DirectX;

class Scene
{
public:
	Scene();
	~Scene();
	void Init();
	void Update();
	void Render();
	void Shutdown();

private:
	Camera* m_camera;
	std::vector<BaseGameObject*> m_gameObjects;
};

