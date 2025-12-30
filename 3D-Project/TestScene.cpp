#include "TestScene.h"

bool TestScene::LoadScene(ID3D11Device* device, ID3D11DeviceContext* context, Window& window)
{
	// Initialize camera
	DirectX::XMFLOAT3 camInitialPos = { 0.0f, 0.0f, -3.0f };
	ProjectionData projData;
	projData.fovInDeg = 90.0f;
	projData.aspectRatio = static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight());
	projData.nearPlane = 0.1f;
	projData.m_farPlane = 1000.0f;

	m_camera = new Camera(device, projData, camInitialPos);

	// Initialize light handler
	m_lightHandler = new LightHandler();

	SpotLightData spotLightData;
	spotLightData.position = DirectX::XMFLOAT3(0.f, -2.f, -5.f);
	spotLightData.intensity = 200.f;
	spotLightData.color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	spotLightData.direction = DirectX::XMFLOAT3(0.f, 0.f, 1.f);
	spotLightData.innerConeInDeg = 15.f;
	spotLightData.outerConeinDeg = 40.f;
	m_lightHandler->AddSpotLight(spotLightData);
	
	m_lightHandler->Init(device, context, m_camera->GetPosition());

	// Initialize game objects
	
	// Strawberry
	Transform strawberryTransform;
	std::string folderPath = "./Objects/Strawberry";
	std::string objectName = "Strawberry_obj.obj";
	std::string textureFolder = "/Texture";
	strawberryTransform.SetPosition(DirectX::XMVectorSet(0.0f, -5.0f, 5.0f, 0.0f));
	strawberryTransform.SetRotation(DirectX::XMVectorSet(-10.0f, 0.f, 0.0f, 0.0f));
	strawberryTransform.SetScale(DirectX::XMVectorSet(1.f, 1.f, 1.f, 0.0f));
	AddGameObject(device, strawberryTransform, folderPath, objectName, textureFolder, true);
	
	return true;
}

void TestScene::UpdateScene(const float deltaTime)
{
	// Rotating strawberry
	DirectX::XMVECTOR rotation = m_gameObjects[0]->GetTransform().GetRotation();
	rotation = DirectX::XMVectorAdd(rotation, DirectX::XMVectorSet(0.0f, 30.f * deltaTime, 0.0f, 0.0f));
	m_gameObjects[0]->GetTransform().SetRotation(rotation);

}
