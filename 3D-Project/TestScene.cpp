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
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, 1.f, -4.f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	std::string folderPath = "./Objects/Cake";
	std::string objectName = "strawberry cake.obj";
	std::string textureFolder = "/TEXTURES";
	this->AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder, true);
	
	// Strawberry2
	strawberryTransform.SetPosition(DirectX::XMVectorSet(-5.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(0.0f, 180.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	folderPath = "./Objects/Cake";
	objectName = "strawberry cake.obj";
	textureFolder = "/TEXTURES";
	this->AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder);

	// Boat
	Transform boatTransform;
	boatTransform.SetPosition(DirectX::XMVectorSet(0.f, 0.0f, 9.0f, 0.0f));
	boatTransform.SetRotation(DirectX::XMVectorSet(0.0f, -90.f, 0.0f, 0.0f));
	boatTransform.SetScale(DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 0.0f));
	folderPath = "./Objects/boat";
	objectName = "boat.obj";
	this->AddGameObject(device, boatTransform, folderPath, objectName, "", false, true);

	// Floor
	Transform floorTransform;
	floorTransform.SetPosition(DirectX::XMVectorSet(0.0f, -15.f, 0.0f, 0.0f));
	floorTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	floorTransform.SetScale(DirectX::XMVectorSet(0.6f, 0.4f, 0.6f, 0.0f));
	folderPath = "./Objects/Desk/metal_table";
	objectName = "metal_table.obj";
	textureFolder = "/textures";
	this->AddGameObject(device, floorTransform, folderPath, objectName, textureFolder, true);

	// Desk
	Transform deskTransform;
	deskTransform.SetPosition(DirectX::XMVectorSet(13.0f, -1.1f, 0.0f, 0.0f));
	deskTransform.SetRotation(DirectX::XMVectorSet(0.0f, 90.f, 0.0f, 0.0f));
	deskTransform.SetScale(DirectX::XMVectorSet(0.05f, 0.05f, 0.05f, 0.0f));
	folderPath = "./Objects/Desk/metal_table";
	objectName = "metal_table.obj";
	textureFolder = "/textures";
	this->AddGameObject(device, deskTransform, folderPath, objectName, textureFolder, false);

	// DCEM Cube
	Transform DCEMTransform;
	DCEMTransform.SetPosition(DirectX::XMVectorSet(0.0f, 2.0f, 2.0f, 0.0f));
	DCEMTransform.SetRotation(DirectX::XMVectorSet(0.0f, 0.f, 0.0f, 0.0f));
	DCEMTransform.SetScale(DirectX::XMVectorSet(1.5f, 1.5f, 1.5f, 0.0f));
	folderPath = "./Objects/SimpleSphere";
	objectName = "Sphere.obj";
	UINT resolution = 512;
	this->AddDCEMObject(device, DCEMTransform, resolution, folderPath, objectName, dcemPS, returnPS);

	// Trees
	const size_t nrOfTrees = 16;
	const float circleRadius = 15.f;
	Transform treeTransform;
	treeTransform.SetScale(DirectX::XMVectorSet(0.3f, 0.3f, 0.3f, 0.0f));
	folderPath = "./Objects/Tree";
	objectName = "ItalianCypress.obj";
	for(size_t i = 0; i < nrOfTrees; ++i)
	{
		float angle = (static_cast<float>(i) / static_cast<float>(nrOfTrees)) * XM_2PI;
		float posX = circleRadius * cosf(angle);
		float posZ = circleRadius * sinf(angle);
		treeTransform.SetPosition(DirectX::XMVectorSet(posX, -0.5f, posZ, 1.0f));
		treeTransform.SetRotation(DirectX::XMVectorSet(0.0f, static_cast<float>(rand() % 360), 0.0f, 0.0f));
		this->AddGameObject(device, treeTransform, folderPath, objectName);
	}
}

void TestScene::LoadSceneLights(ID3D11Device* device, ID3D11DeviceContext* context)
{
	using namespace DirectX;
	
	if (m_lightHandler)
		m_lightHandler.reset();

	const UINT spotLightResolution = 1024;
	const UINT dirLightResolution = 2048;
	m_lightHandler = std::make_unique<LightHandler>(spotLightResolution, dirLightResolution);

	// === Spot Lights ===

	// Front facing spot light
	SpotLightData spotLightData = {};
	spotLightData.position = XMFLOAT3(0.f, 1.f, -12.f);
	spotLightData.direction = XMFLOAT3(0.f, 0.f, 1.f);
	spotLightData.intensity = 60.f;
	spotLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	spotLightData.innerConeInDeg = 5.f;
	spotLightData.outerConeinDeg = 25.f;
	spotLightData.range = 30.f;
	m_lightHandler->AddSpotLight(spotLightData);

	Transform lightTransform;
	XMVECTOR lightPos = XMLoadFloat3(&spotLightData.position);
	lightTransform.SetPosition(lightPos);
	lightTransform.SetRotation(DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f));
	lightTransform.SetScale(DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 0.f));
	std::string folderPath = "./Objects/Bulb";
	std::string objectName = "BULB.obj";
	this->AddGameObject(device, lightTransform, folderPath, objectName);

	// Table facing spot light
	SpotLightData spotLightData2 = {};
	spotLightData2.position = XMFLOAT3(10.f, 3.f, 0.f);
	spotLightData2.direction = XMFLOAT3(0.8f, -1.f, 0.f);
	spotLightData2.intensity = 100.f;
	spotLightData2.color = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
	spotLightData2.innerConeInDeg = 5.f;
	spotLightData2.outerConeinDeg = 30.f;
	spotLightData2.range = 20.f;
	m_lightHandler->AddSpotLight(spotLightData2);

	lightTransform;
	lightPos = XMLoadFloat3(&spotLightData2.position);
	lightTransform.SetPosition(lightPos);
	lightTransform.SetRotation(DirectX::XMVectorSet(0.f, 90.f, 0.f, 0.f));
	lightTransform.SetScale(DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 0.f));
	folderPath = "./Objects/Bulb";
	objectName = "BULB.obj";
	this->AddGameObject(device, lightTransform, folderPath, objectName);

	// === Directional Lights ===
	// Top down directional light
	DirectionalLightData dirLightData = {};
	dirLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	dirLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	dirLightData.intensity = 0.6f;
	m_lightHandler->AddDirectionalLight(dirLightData);

	// Down left directional light
	dirLightData.color = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.f);
	dirLightData.direction = XMFLOAT3(-1.f, -1.f, 0.f);
	dirLightData.intensity = 0.7f;
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

