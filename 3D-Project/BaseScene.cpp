#include "BaseScene.h"
#include "GameObject.h"
#include "DCEM.h"

#include <algorithm>
#include <memory>

void BaseScene::Init(ID3D11Device* device, ID3D11DeviceContext* context, const Window* window)
{	
	this->window = const_cast<Window*>(window);
	m_quadTree = QuadTree<BaseObject>();
	LoadScene(device, context, window->GetWidth(), window->GetHeight());
}

void BaseScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height)
{
	LoadSceneCameras(device, context, width, height);
	LoadSceneGameObjects(device, context);
	LoadSceneLights(device, context);
}

void BaseScene::UpdateSceneLights(ID3D11DeviceContext* context)
{
	if (m_lightHandler && m_camera)
	{
		DirectX::XMFLOAT3 camPosF3;
		DirectX::XMStoreFloat3(&camPosF3, m_camera->GetPosition());
		m_lightHandler->UpdateLightBuffer(context, camPosF3);
	}
}

Camera* BaseScene::GetCamera() const
{
	return m_camera.get();
}

LightHandler* BaseScene::GetLightHandler() const
{
	return m_lightHandler.get();
}

void BaseScene::BindLights(ID3D11DeviceContext* context)
{
	if (m_lightHandler)
		m_lightHandler->BindLightBuffer(context);
}

void BaseScene::AddBaseObject(std::unique_ptr<BaseObject> baseObject)
{
	if (baseObject)
		m_sceneObjects.emplace_back(std::move(baseObject));
}

void BaseScene::AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, std::string& objectName, const std::string& textureFolder, const bool flipUVy)
{
	auto newBaseObject = std::make_unique<GameObject>(device, transform, folderPath, objectName, textureFolder, flipUVy);
	m_sceneObjects.emplace_back(std::move(newBaseObject));
}

void BaseScene::AddDCEMObject(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName)
{
	auto newDCEM = std::make_unique<DCEM>(device, transform, resolution, folderPath, objectName);
	m_dcemObjects.emplace_back(std::move(newDCEM));
}

std::vector<std::unique_ptr<BaseObject>>& BaseScene::GetSceneObjects()
{
	return m_sceneObjects;
}

std::vector<std::unique_ptr<DCEM>>& BaseScene::GetDCEMObjects()
{
	return m_dcemObjects;
}

BaseScene::~BaseScene() = default;
