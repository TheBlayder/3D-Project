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

class Scene
{
private:
	Camera* m_camera;
	Player* m_player;
	std::vector<GameObject*> m_gameObjects;

public:
	Scene();
	~Scene();
	void Init();
	void Update();
	void RenderScene();
	void Shutdown();

	Camera* GetCamera() const;
	void SetCamera(Camera* camera);
	
	Player* GetPlayer() const;
	void SetPlayer(Player* player);

	void AddGameObject(GameObject* gameObject);
	std::vector<GameObject*> GetGameObjects() const;
};