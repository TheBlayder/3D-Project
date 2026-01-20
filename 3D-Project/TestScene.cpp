#include "TestScene.h"

TestScene::~TestScene() {}

void TestScene::UpdateScene(const float deltaTime, ID3D11DeviceContext* context)
{
	using namespace DirectX;

	m_camera->Update(this->GetWindow()->GetInputHandler(), deltaTime);

	if (this->IsScenePaused()) // If the scene is paused, only update the camera
		return;

	m_particleHandler.Update(context, deltaTime);

	for(auto& dcem : m_dcemObjects)
	{
		dcem->Update(context, deltaTime, m_camera.get());
	}

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

void TestScene::LoadSceneGameObjects(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS)
{
	using namespace DirectX;

	// Strawberry
	Transform strawberryTransform;
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, 1.f, -4.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	std::string folderPath = "./Objects/Cake";
	std::string objectName = "strawberry cake.obj";
	std::string textureFolder = "/TEXTURES";
	//AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder);

	// Boat
	Transform boatTransform;
	boatTransform.SetPosition(DirectX::XMVectorSet(0.f, 0.0f, 8.0f, 0.0f));
	boatTransform.SetRotation(DirectX::XMVectorSet(0.0f, -90.f, 0.0f, 0.0f));
	boatTransform.SetScale(DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f));
	folderPath = "./Objects/boat";
	objectName = "boat.obj";
	AddGameObject(device, boatTransform, folderPath, objectName, "", false);

	// Floor
	Transform floorTransform;
	floorTransform.SetPosition(DirectX::XMVectorSet(0.0f, -19.f, 0.0f, 0.0f));
	floorTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	floorTransform.SetScale(DirectX::XMVectorSet(0.7f, 0.2f, 0.7f, 0.0f));
	folderPath = "./Objects/Desk/metal_table";
	objectName = "metal_table.obj";
	textureFolder = "/textures";
	//AddGameObject(device, floorTransform, folderPath, objectName, textureFolder);

	// Desk
	Transform deskTransform;
	deskTransform.SetPosition(DirectX::XMVectorSet(20.0f, -1.1f, 0.0f, 0.0f));
	deskTransform.SetRotation(DirectX::XMVectorSet(0.0f, 90.f, 0.0f, 0.0f));
	deskTransform.SetScale(DirectX::XMVectorSet(0.05f, 0.05f, 0.05f, 0.0f));
	folderPath = "./Objects/Desk/metal_table";
	objectName = "metal_table.obj";
	textureFolder = "/textures";
	AddGameObject(device, deskTransform, folderPath, objectName, textureFolder, false);

	// DCEM Cube
	Transform DCEMTransform;
	DCEMTransform.SetPosition(DirectX::XMVectorSet(0.0f, 2.0f, 2.0f, 0.0f));
	DCEMTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	DCEMTransform.SetScale(DirectX::XMVectorSet(1.5f, 1.5f, 1.5f, 0.0f));
	folderPath = "./Objects/SimpleSphere";
	objectName = "Sphere.obj";
	UINT resolution = 512;
	AddDCEMObject(device, DCEMTransform, resolution, folderPath, objectName, dcemPS, returnPS);
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
	spotLightData.position = XMFLOAT3(0.f, 1.f, -15.f);
	spotLightData.direction = XMFLOAT3(0.f, 0.f, 1.f);
	spotLightData.intensity = 90.f;
	spotLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	spotLightData.innerConeInDeg = 10.f;
	spotLightData.outerConeinDeg = 30.f;
	spotLightData.range = 30.f;
	m_lightHandler->AddSpotLight(spotLightData);

	Transform lightTransform;
	XMVECTOR lightPos = XMLoadFloat3(&spotLightData.position);
	lightTransform.SetPosition(lightPos);
	lightTransform.SetRotation(DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f));
	lightTransform.SetScale(DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 0.f));
	std::string folderPath = "./Objects/Bulb";
	std::string objectName = "BULB.obj";
	AddGameObject(device, lightTransform, folderPath, objectName);

	DirectionalLightData dirLightData = {};
	dirLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	dirLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	dirLightData.intensity = 0.9f;
	m_lightHandler->AddDirectionalLight(dirLightData);

	DirectX::XMFLOAT3 camPosF3;
	DirectX::XMStoreFloat3(&camPosF3, m_camera->GetPosition());
	m_lightHandler->Init(device, context, camPosF3);
}

void TestScene::InitializeParticles(ID3D11Device* device, ID3D11DeviceContext* context, bool hasParticles)
{
	if(hasParticles == false)
		return;

	m_hasParticles = true;
	bool isDynamic = false;
	bool hasSRV = true;
	bool hasUAV = true;
	UINT nrOfParticles = 2000;

	m_particleHandler.Init(device, context, nrOfParticles, isDynamic, hasSRV, hasUAV);
}

