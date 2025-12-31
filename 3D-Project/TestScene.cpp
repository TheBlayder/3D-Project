#include "TestScene.h"

bool TestScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{
	using namespace DirectX;

	// Initialize camera
	XMFLOAT3 camInitialPos = { 0.0f, 0.0f, -2.0f };
	ProjectionData projData;
	projData.fovInDeg = 90.0f;
	projData.aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 1000.0f;

	m_camera = new Camera(device, projData, camInitialPos);

	// Initialize light handler
	m_lightHandler = new LightHandler();

	SpotLightData spotLightData = {};
	spotLightData.position = XMFLOAT3(0.f, -1.f, -5.f);
	spotLightData.intensity = 10.f;
	spotLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	spotLightData.direction = XMFLOAT3(0.f, 0.f, 1.f);
	spotLightData.innerConeInDeg = 1.f;
	spotLightData.outerConeinDeg = 30.f;
	spotLightData.range = 20.f;
	m_lightHandler->AddSpotLight(spotLightData);

	DirectionalLightData dirLightData = {};
	dirLightData.color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	dirLightData.direction = XMFLOAT3(0.f, -1.f, 0.f);
	dirLightData.intensity = 1.f;
	m_lightHandler->AddDirectionalLight(dirLightData);
	
	m_lightHandler->Init(device, context, m_camera->GetPosition());

	// Initialize game objects
	
	// Strawberry
	Transform strawberryTransform;
	std::string folderPath = "./Objects/Cake";
	std::string objectName = "strawberry cake.obj";
	std::string textureFolder = "/TEXTURES";
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, -1.f, 0.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(-10.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder, true);
	
	return true;
}

void TestScene::UpdateScene(const float deltaTime)
{
	// Rotating strawberry
	float rotationSpeed = 30.f;
	DirectX::XMVECTOR rotation = m_gameObjects[0]->GetTransform().GetRotation();
	rotation = DirectX::XMVectorAdd(rotation, DirectX::XMVectorSet(0.0f, rotationSpeed * deltaTime, 0.0f, 0.0f));
	m_gameObjects[0]->GetTransform().SetRotation(rotation);

}
