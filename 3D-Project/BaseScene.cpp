#include "BaseScene.h"

Camera* BaseScene::GetCamera() const
{
    return m_camera;
}

void BaseScene::AddGameObject(GameObject* gameObject)
{
	m_gameObjects.push_back(gameObject);
}

void BaseScene::AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder = "")
{
	GameObject* newGameObject = new GameObject();
	newGameObject->Init(device, transform, folderPath, objectName, textureFolder);
	m_gameObjects.push_back(newGameObject);
}

std::vector<GameObject*>& BaseScene::GetGameObjects()
{
	return m_gameObjects;
}
