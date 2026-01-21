#include "BaseScene.h"
#include "GameObject.h"
#include "DCEM.h"

#include "HelperFuncs.h"

#include <algorithm>
#include <memory>

void BaseScene::Init(ID3D11Device* device, ID3D11DeviceContext* context, Window* window, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, bool hasParticles)
{	
	m_window = window;

	size_t quadTreeMaxDepth = 8;
	m_quadTree = QuadTree<BaseObject>(quadTreeMaxDepth);
	LoadScene(device, context, window->GetWidth(), window->GetHeight(), dcemPS, returnPS, hasParticles);
}

void BaseScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height,
	ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, bool hasParticles)
{
	LoadSceneCameras(device, context, width, height);
	LoadSceneGameObjects(device, context, dcemPS, returnPS);
	LoadSceneLights(device, context);
	InitializeParticles(device, context, hasParticles);
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

void BaseScene::AddGameObject(ID3D11Device* device, const Transform& transform, std::string& folderPath, 
	std::string& objectName, const std::string& textureFolder, const bool dynamic, const bool tesselate, const bool flipUVy)
{
	GameObject* object = new GameObject(device, transform, folderPath, objectName, textureFolder, tesselate, flipUVy);
	std::unique_ptr<BaseObject> newBaseObject(object);

	m_sceneObjects.emplace_back(std::move(newBaseObject));
	BaseObject* objectPtr = m_sceneObjects.back().get();

	if (!dynamic)
	{
		m_quadTree.AddElement(objectPtr);
	}
	else
	{
		m_dynamicObjects.emplace_back(objectPtr);
	}
}

void BaseScene::AddDCEMObject(ID3D11Device* device, const Transform& transform, const UINT& resolution, 
	std::string& folderPath, std::string& objectName, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS)
{
	DCEM* dcemObject = new DCEM(device, transform, resolution, folderPath, objectName, dcemPS, returnPS);
	m_dcemObjects.emplace_back(dcemObject);
	
	std::unique_ptr<BaseObject> newDCEM(dcemObject);
	m_sceneObjects.emplace_back(std::move(newDCEM));

	BaseObject* dcemPtr = m_sceneObjects.back().get();
	m_quadTree.AddElement(dcemPtr);
}

std::vector<std::unique_ptr<BaseObject>>& BaseScene::GetSceneObjects()
{
	return m_sceneObjects;
}

std::vector<DCEM*>& BaseScene::GetDCEMObjects()
{
	return m_dcemObjects;
}

std::vector<BaseObject*> BaseScene::GetVisableSceneObjects(Camera* camera)
{
    using namespace DirectX;

	ProjectionData projData = camera->GetProjectionData();
	projData.fovInDeg -= 30.f;

    BoundingFrustum frustum;
	XMFLOAT4X4 projMatrixF4; 
	MatrixHelper::CreateProjectionMatrix(projMatrixF4, projData.fovInDeg, projData.aspectRatio, projData.nearPlane, projData.m_farPlane);
    XMMATRIX projMatrix = XMLoadFloat4x4(&projMatrixF4);
    BoundingFrustum::CreateFromMatrix(frustum, projMatrix); 

    XMFLOAT4X4 viewMatrixF4 = camera->GetViewMatrix();
    XMMATRIX worldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrixF4));
    BoundingFrustum worldFrustum;
    frustum.Transform(worldFrustum, worldMatrix);
    frustum = worldFrustum;

	std::vector<BaseObject*> objects = m_quadTree.CheckTree(frustum);

	for (auto& dynamicObj : m_dynamicObjects)
	{
		objects.emplace_back(dynamicObj);
	}

	return objects;
}

BaseScene::~BaseScene() = default;
