#include "BaseScene.h"

#include <algorithm>

void BaseScene::Init(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{	
	LoadScene(device, context, window);
}

void BaseScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{
	LoadSceneCameras(device, context, window);
	LoadSceneGameObjects(device, context);
	LoadSceneLights(device, context);
}

void BaseScene::UpdateSceneLights(ID3D11DeviceContext* context)
{
	m_lightHandler->UpdateLightBuffer(context, m_camera->GetPosition());
}

Camera* BaseScene::GetCamera() const
{
	return m_camera;
}

LightHandler* BaseScene::GetLightHandler() const
{
	return m_lightHandler;
}

void BaseScene::BindLights(ID3D11DeviceContext* context)
{
	m_lightHandler->BindLightBuffer(context);
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

BaseScene::~BaseScene()
{
	// Clean up owned pointers
	if (m_camera)
		delete m_camera;

	if (m_lightHandler)
		delete m_lightHandler;

	for (auto obj : m_gameObjects)
	{
		delete obj;
	}
	m_gameObjects.clear();
}
