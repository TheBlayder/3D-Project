#include "BaseScene.h"

bool BaseScene::Init(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{
	m_camera = new Camera();
	m_lightHandler = new LightHandler();
	if (!LoadScene(device, context, window)) { return false; }
}

void BaseScene::UpdateSceneLights(ID3D11DeviceContext* context)
{
	if (m_lightHandler)
	{
		m_lightHandler->UpdateLightBuffer(context, m_camera->GetPosition());
	}
}

Camera* BaseScene::GetCamera() const
{
    return m_camera;
}

void BaseScene::BindLights(ID3D11DeviceContext* context)
{
	if (m_lightHandler) { m_lightHandler->BindLightBuffer(context); }
}

void BaseScene::AddGameObject(GameObject* gameObject)
{
	m_gameObjects.push_back(gameObject);
}

void BaseScene::AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder, const bool flipUVy)
{
	GameObject* newGameObject = new GameObject();
	newGameObject->Init(device, transform, folderPath, objectName, textureFolder, flipUVy);
	m_gameObjects.push_back(newGameObject);
}

std::vector<GameObject*>& BaseScene::GetGameObjects()
{
	return m_gameObjects;
}
