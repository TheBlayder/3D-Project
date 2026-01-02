#include "TestScene.h"

void TestScene::UpdateScene(const float deltaTime)
{
	using namespace DirectX;

	// Rotating strawberry
	float rotationSpeed = 30.f;
	XMVECTOR strawberryRotation = m_gameObjects[0]->GetTransform().GetRotation();
	strawberryRotation = XMVectorAdd(strawberryRotation, XMVectorSet(0.0f, rotationSpeed * deltaTime, 0.0f, 0.0f));
	m_gameObjects[0]->GetTransform().SetRotation(strawberryRotation);
}

void TestScene::LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{
	using namespace DirectX;

	XMFLOAT3 camInitialPos = { 0.0f, 0.0f, -3.0f };
	ProjectionData projData;
	projData.fovInDeg = 90.0f;
	projData.aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 1000.0f;

	if (m_camera) delete m_camera;
	m_camera = new Camera(device, projData, camInitialPos);
}

void TestScene::LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context)
{
	using namespace DirectX;

	// Strawberry
	Transform strawberryTransform;
	std::string folderPath = "./Objects/Cake";
	std::string objectName = "strawberry cake.obj";
	std::string textureFolder = "/TEXTURES";
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder);

	// Floor
	Transform floorTransform;
	folderPath = "./Objects/Cube";
	objectName = "cube.obj";
	floorTransform.SetPosition(DirectX::XMVectorSet(0.0f, -1.5f, 0.0f, 0.0f));
	floorTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	floorTransform.SetScale(DirectX::XMVectorSet(10.f, 0.5f, 10.f, 0.0f));
	AddGameObject(device, floorTransform, folderPath, objectName);
}

void TestScene::LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context)
{
	using namespace DirectX;
	
	if (m_lightHandler) delete m_lightHandler;

	const UINT spotLightResolution = 1024;
	const UINT dirLightResolution = 2048;
	m_lightHandler = new LightHandler(spotLightResolution, dirLightResolution);

	SpotLightData spotLightData = {};
	spotLightData.position = XMFLOAT3(0.f, 7.f, 0.f);
	spotLightData.intensity = 50.f;
	spotLightData.color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	spotLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	spotLightData.innerConeInDeg = 10.f;
	spotLightData.outerConeinDeg = 30.f;
	spotLightData.range = 20.f;
	m_lightHandler->AddSpotLight(spotLightData);

	DirectionalLightData dirLightData = {};
	dirLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	dirLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	dirLightData.intensity = 1.f;
	m_lightHandler->AddDirectionalLight(dirLightData);

	m_lightHandler->Init(device, context, m_camera->GetPosition());
}

