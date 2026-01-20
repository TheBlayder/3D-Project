#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "ConstantBuffer.h"
#include "Transform.h"
#include "GBuffer.h"
#include "DeferredHandler.h"
#include "Window.h"

namespace DX = DirectX;

struct ProjectionData
{
	float fovInDeg = 0.f;
	float aspectRatio = 0.f;
	float nearPlane = 0.f;
	float m_farPlane = 0.f;
};

class Camera
{
private:
	DeferredHandler* m_DH = nullptr;
	D3D11_VIEWPORT m_viewport;

	Transform m_transform;
	DX::XMVECTOR m_up = { 0.f, 1.f, 0.f, 0.f };

	ProjectionData m_projData;
	ConstantBuffer* m_cameraBuffer = nullptr;
	
	void MoveInDirection(float amount, const DirectX::XMVECTOR& direction);
	
	void GenerateViewMatrix(DX::XMFLOAT4X4& viewMatrix);
	void GenerateProjectionMatrix(DX::XMFLOAT4X4& projMatrix);
	void GenerateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix);

	const float m_cameraSpeed = 10.f;

public:
	Camera() = default;
	Camera(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.f, 0.f, 0.f));
	~Camera();

	void Init(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.f, 0.f, 0.f));

	void RotateAroundAxis(float amount, const DirectX::XMVECTOR& axis);

	void Update(InputHandler& input, float deltaTime);

	// Constant buffer
	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	DirectX::XMFLOAT4X4 GetViewProjMatrix();
	DeferredHandler* GetDeferredHandler();

	Transform& GetTransform() { return m_transform; }

	ProjectionData GetProjectionData() { return m_projData; }

	DX::XMVECTOR GetForward() const;
	DX::XMVECTOR GetPosition() const;
	DX::XMVECTOR GetRight() const;
	DX::XMVECTOR GetUp() const;

	float GetFov() const;
	void SetFov(float fovInDeg);

	D3D11_VIEWPORT& GetViewport() { return m_viewport; }
};