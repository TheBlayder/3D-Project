#include "TestScene.h"

void TestScene::UpdateScene(const float deltaTime)
{
	using namespace DirectX;

	m_camera->Update(this->GetWindow()->GetInputHandler(), deltaTime);

	// Rotating strawberry
	float rotationSpeed = 30.f;
	XMVECTOR strawberryRotation = m_sceneObjects[0]->GetTransform().GetRotation();
	strawberryRotation = XMVectorAdd(strawberryRotation, XMVectorSet(0.0f, rotationSpeed * deltaTime, 0.0f, 0.0f));
	m_sceneObjects[0]->GetTransform().SetRotation(strawberryRotation);
}

void TestScene::LoadSceneCameras(ID3D11Device* device, ID3D11DeviceContext* context, const UINT width, const UINT height)
{
	using namespace DirectX;

	XMFLOAT3 camInitialPos = { 0.0f, 3.0f, -5.0f };
	ProjectionData projData;
	projData.fovInDeg = 90.0f;
	projData.aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 1000.0f;

	if (m_camera)
		m_camera.reset();
	m_camera = std::make_unique<Camera>(device, projData, width, height, camInitialPos);
}

void TestScene::LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context)
{
	using namespace DirectX;

	// Strawberry
	Transform strawberryTransform;
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, 1.f, 0.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	std::string folderPath = "./Objects/Cake";
	std::string objectName = "strawberry cake.obj";
	std::string textureFolder = "/TEXTURES";
	AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder);

	// Floor
	Transform floorTransform;
	floorTransform.SetPosition(DirectX::XMVectorSet(0.0f, -1.5f, 0.0f, 0.0f));
	floorTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	floorTransform.SetScale(DirectX::XMVectorSet(10.f, 0.5f, 10.f, 0.0f));
	folderPath = "./Objects/Cube";
	objectName = "cube.obj";
	AddGameObject(device, floorTransform, folderPath, objectName);

	// DCEM Cube
	Transform DCEMTransform;
	DCEMTransform.SetPosition(DirectX::XMVectorSet(0.0f, 3.0f, 7.0f, 0.0f));
	DCEMTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	DCEMTransform.SetScale(DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f));
	folderPath = "./Objects/Cube";
	objectName = "cube.obj";
	UINT resolution = 256;
	//AddDCEMObject(device, DCEMTransform, resolution, folderPath, objectName);

}

void TestScene::LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context)
{
	using namespace DirectX;
	
	if (m_lightHandler)
		m_lightHandler.reset();

	const UINT spotLightResolution = 1024;
	const UINT dirLightResolution = 2048;
	m_lightHandler = std::make_unique<LightHandler>(spotLightResolution, dirLightResolution);

	SpotLightData spotLightData = {};
	spotLightData.position = XMFLOAT3(0.f, 1.f, -10.f);
	spotLightData.direction = XMFLOAT3(0.f, 0.f, 1.f);
	spotLightData.intensity = 90.f;
	spotLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	spotLightData.innerConeInDeg = 10.f;
	spotLightData.outerConeinDeg = 30.f;
	spotLightData.range = 30.f;
	m_lightHandler->AddSpotLight(spotLightData);

	DirectionalLightData dirLightData = {};
	dirLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	dirLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	dirLightData.intensity = 0.9f;
	m_lightHandler->AddDirectionalLight(dirLightData);

	DirectX::XMFLOAT3 camPosF3;
	DirectX::XMStoreFloat3(&camPosF3, m_camera->GetPosition());
	m_lightHandler->Init(device, context, camPosF3);
}

