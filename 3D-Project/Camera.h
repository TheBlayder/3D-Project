#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "ConstantBuffer.h"
#include "Transform.h"
#include "GBuffer.h"
#include "DeferredHandler.h"

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
	DeferredHandler* m_DH;

	Transform m_transform;
	DX::XMVECTOR m_up = { 0.f, 1.f, 0.f, 0.f };

	ProjectionData m_projData;
	ConstantBuffer* m_cameraBuffer;
	
	void MoveInDirection(float amount, const DirectX::XMFLOAT3& direction);
	void RotateAroundAxis(float amount);

	void GenerateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix);

public:
	Camera() = default;
	Camera(ID3D11Device* device, ProjectionData& projData, const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.f, 0.f, 0.f));
	~Camera() = default;

	// Movement (needs redo)
	void MoveForward(float amount); // Forward & Backward
	void RotateRight(float amount);

	// Constant buffer
	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	// Getters
	const DirectX::XMFLOAT4X4 GetViewProjMatrix();

	const DX::XMFLOAT3& GetForward() const;
	const DX::XMFLOAT3& GetPosition() const;
	const DX::XMFLOAT3& GetRight() const;
	const DX::XMFLOAT3& GetUp() const;
};