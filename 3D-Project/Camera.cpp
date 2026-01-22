#include "Camera.h"
#include "HelperFuncs.h"

namespace MH = MatrixHelper;

void Camera::MoveInDirection(float amount, const DirectX::XMVECTOR& direction)
{
	m_transform.SetPosition(DirectX::XMVectorAdd(m_transform.GetPosition(), DirectX::XMVectorScale(direction, amount)));
}

void Camera::RotateAroundAxis(float amount, const DirectX::XMVECTOR& axis)
{
	using namespace DirectX;
	XMVECTOR cameraRotation = m_transform.GetRotation();
	cameraRotation = XMVectorAdd(cameraRotation, DirectX::XMVectorScale(axis, amount));
	m_transform.SetRotation(cameraRotation);
}

void Camera::GenerateViewMatrix(DX::XMFLOAT4X4& viewMatrix)
{
	using namespace DirectX;
	MH::CreateViewMatrix(viewMatrix, m_transform.GetPosition(), GetForward(), m_up);
}

void Camera::GenerateProjectionMatrix(DX::XMFLOAT4X4& projMatrix)
{
	using namespace DirectX;
	MH::CreateProjectionMatrix(projMatrix, m_projData.fovInDeg, m_projData.aspectRatio, m_projData.nearPlane, m_projData.m_farPlane);
}

void Camera::GenerateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix)
{
	using namespace DirectX;
	XMFLOAT4X4 viewMatrix, projMatrix;

	MH::CreateViewMatrix(viewMatrix, m_transform.GetPosition(), GetForward(), m_up);
	MH::CreateProjectionMatrix(projMatrix, m_projData.fovInDeg, m_projData.aspectRatio, m_projData.nearPlane, m_projData.m_farPlane);
	MH::CreateViewProjMatrix(viewProjMatrix, viewMatrix, projMatrix);
}

Camera::Camera(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition)
{
	Init(device, projData, width, height, initialPosition);
}

void Camera::Init(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition)
{
	m_projData = projData;
	m_DH = std::make_unique<DeferredHandler>(device, width, height);

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<FLOAT>(width);
	m_viewport.Height = static_cast<FLOAT>(height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	
	using namespace DirectX;
	m_transform.SetPosition(XMLoadFloat3(&initialPosition));
	m_transform.SetRotation(XMVectorSet(0.f, 0.f, 0.f, 0.f));

	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
	m_cameraBuffer = std::make_unique<ConstantBuffer>(device, sizeof(DX::XMFLOAT4X4), &viewProjMatrix);
}


void Camera::Update(InputHandler& input, float deltaTime)
{
	const float moveAmount = m_cameraSpeed * deltaTime;
	const float rotateAmount = 75.0f * deltaTime; // 90 degrees per second

	if (input.isDown('W'))
		MoveInDirection(-moveAmount, GetForward());
	if (input.isDown('S'))
		MoveInDirection(moveAmount, GetForward());
	if (input.isDown('A'))
		MoveInDirection(-moveAmount, GetRight());
	if (input.isDown('D'))
		MoveInDirection(moveAmount, GetRight());

	if (input.isDown(VK_SPACE))
		MoveInDirection(moveAmount, GetUp());
	if (input.isDown(VK_CONTROL))
		MoveInDirection(-moveAmount, GetUp());

	// Rotate around Y-axis
	if (input.isDown('Q')) // Rotate left
	{
		using namespace DirectX;
		XMVECTOR rot = m_transform.GetRotation();
		float pitch = XMVectorGetX(rot);
		float yaw = XMVectorGetY(rot);
		float roll = XMVectorGetZ(rot);
		yaw += rotateAmount;
		m_transform.SetRotation(XMVectorSet(pitch, yaw, roll, 0.f));
	}
	if (input.isDown('E')) // Rotate right
	{
		using namespace DirectX;
		XMVECTOR rot = m_transform.GetRotation();
		float pitch = XMVectorGetX(rot);
		float yaw = XMVectorGetY(rot);
		float roll = XMVectorGetZ(rot);
		yaw -= rotateAmount;
		m_transform.SetRotation(XMVectorSet(pitch, yaw, roll, 0.f));
	}
}

// === CONSTANT BUFFER ===
void Camera::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	using namespace DirectX;
	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
	m_cameraBuffer->Update(context, &viewProjMatrix);
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 viewMatrix;
	GenerateViewMatrix(viewMatrix);
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 projMatrix;
	GenerateProjectionMatrix(projMatrix);
	return projMatrix;
}

// === GETTERS ===
DirectX::XMFLOAT4X4 Camera::GetViewProjMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
	return viewProjMatrix;
}

DeferredHandler* Camera::GetDeferredHandler()
{
	return m_DH.get();
}

DirectX::XMVECTOR Camera::GetForward() const
{
	using namespace DirectX;

	XMVECTOR rot = m_transform.GetRotation();
	float pitch = XMConvertToRadians(XMVectorGetX(rot));
	float yaw   = XMConvertToRadians(XMVectorGetY(rot));
	float roll  = XMConvertToRadians(XMVectorGetZ(rot));

	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), rotMat);
	forward = XMVector3Normalize(forward);
	return forward;
}

DirectX::XMVECTOR Camera::GetPosition() const
{
	return m_transform.GetPosition();
}

DirectX::XMVECTOR Camera::GetRight() const
{
	using namespace DirectX;

	const XMVECTOR worldUp = m_up;
	XMVECTOR forward = GetForward();
	XMVECTOR right = XMVector3Cross(forward, worldUp);
	right = XMVector3Normalize(right);

	return right;
}

DirectX::XMVECTOR Camera::GetUp() const
{
	using namespace DirectX;

	XMVECTOR forward = GetForward();
	XMVECTOR right = GetRight();

	XMVECTOR up = XMVector3Cross(forward, right);
	up = XMVector3Normalize(up);

	return up;
}

float Camera::GetFov() const
{
	return m_projData.fovInDeg;
}

void Camera::SetFov(float fovInDeg)
{
	m_projData.fovInDeg = fovInDeg;
}
