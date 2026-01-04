#include "BaseScene.h"
#include "GameObject.h"
#include "DCEM.h"

#include <algorithm>

void BaseScene::Init(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height)
{	
	LoadScene(device, context, width, height);
}

void BaseScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height)
{
	LoadSceneCameras(device, context, width, height);
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

void BaseScene::AddBaseObject(BaseObject* baseObject)
{
	m_sceneObjects.push_back(baseObject);
}

void BaseScene::AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder, const bool flipUVy)
{
	BaseObject* newBaseObject = new GameObject(device, transform, folderPath, objectName, textureFolder, flipUVy);
	m_sceneObjects.push_back(newBaseObject);
}

void BaseScene::AddDCEMObject(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName)
{
	DCEM* newDCEM = new DCEM(device, transform, resolution, folderPath, objectName);
	m_dcemObjects.push_back(newDCEM);
}

std::vector<BaseObject*>& BaseScene::GetSceneObjects()
{
	return m_sceneObjects;
}

std::vector<DCEM*>& BaseScene::GetDCEMObjects()
{
	return m_dcemObjects;
}

BaseScene::~BaseScene()
{
	// Clean up owned pointers
	if (m_camera)
		delete m_camera;

	if (m_lightHandler)
		delete m_lightHandler;

	for (auto obj : m_sceneObjects)
	{
		delete obj;
	}
	m_sceneObjects.clear();

	for (auto dcem : m_dcemObjects)
	{
		delete dcem;
	}
	m_dcemObjects.clear();
}
