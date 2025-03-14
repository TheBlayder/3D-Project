#pragma once

#include "d3d11.h"
#include "DirectXMath.h"
#include "ConstantBuffer.h"

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
	DX::XMFLOAT3 m_position = {0.f, 0.f, 0.f};
	DX::XMFLOAT3 m_forward = { 0.f, 0.f, 1.f }; // Make sure this is normalized
	DX::XMFLOAT3 m_right = {1.f, 0.f, 0.f};
	DX::XMFLOAT3 m_up = { 0.f, 1.f, 0.f };

	ProjectionData m_projData;

	ConstantBuffer* m_cameraBuffer;
	
	void MoveInDirection(float amount, const DirectX::XMFLOAT3& direction);
	void RotateAroundAxis(float amount, const DirectX::XMFLOAT3& axis);

public:
	Camera() = default;
	Camera(ID3D11Device* device, ProjectionData& projData, const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.f, 0.f, 0.f));
	~Camera();

	// Movement
	void MoveForward(float amount); // Forward & Backward
	void MoveRight(float amount); // Right & Left

	void RotateRight(float amount); // Side to side

	// Constant buffer
	void UpdateConstantBuffer(ID3D11DeviceContext* context);

	// Getters
	ID3D11Buffer* GetConstantBuffer() const;
	const DX::XMFLOAT3& GetPosition() const;
	const DX::XMFLOAT3& GetForward() const;
	const DX::XMFLOAT3& GetRight() const;
	const DX::XMFLOAT3& GetUp() const;
};

