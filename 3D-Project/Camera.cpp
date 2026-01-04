#include "Camera.h"
#include "HelperFuncs.h"

namespace MH = MatrixHelper;

void Camera::MoveInDirection(float amount, const DirectX::XMFLOAT3& direction)
{
	m_transform.SetPosition(DirectX::XMVectorAdd(m_transform.GetPosition(), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), amount)));
}

void Camera::GenerateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix)
{
	using namespace DirectX;
	XMFLOAT4X4 viewMatrix, projMatrix;

	MH::CreateViewMatrix(viewMatrix, m_transform.GetPosition(), m_transform.GetRotation(), m_up);
	MH::CreateProjectionMatrix(projMatrix, m_projData.fovInDeg, m_projData.aspectRatio, m_projData.nearPlane, m_projData.m_farPlane);
	MH::CreateViewProjMatrix(viewProjMatrix, viewMatrix, projMatrix);
}

Camera::Camera(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition)
{
	Init(device, projData, width, height, initialPosition);
}

Camera::~Camera()
{
	if (m_cameraBuffer != nullptr)
	{
		delete m_cameraBuffer;
		m_cameraBuffer = nullptr;
	}
	
	if (m_DH != nullptr)
	{
		delete m_DH;
		m_DH = nullptr;
	}
}

void Camera::Init(ID3D11Device* device, ProjectionData& projData, const UINT width, const UINT height, const DX::XMFLOAT3& initialPosition)
{
	m_projData = projData;
	m_DH = new DeferredHandler(device, width, height);
	
	using namespace DirectX;
	m_transform.SetPosition(XMLoadFloat3(&initialPosition));
	m_transform.SetRotation(XMVectorSet(0.f, 0.f, 0.001f, 0.f));

	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
	m_cameraBuffer = new ConstantBuffer(device, sizeof(DX::XMFLOAT4X4), &viewProjMatrix);
}

// === MOVEMENT ===
void Camera::MoveForward(float amount)
{
	MoveInDirection(amount, m_transform.GetRotationF3());
}

void Camera::RotateAroundAxis(float amount, const DirectX::XMFLOAT3& axis)
{
	using namespace DirectX;
	XMVECTOR currentRotation = m_transform.GetRotation();
	XMVECTOR rotationAmount = XMVectorScale(XMLoadFloat3(&axis), amount);
	m_transform.SetRotation(XMVectorAdd(currentRotation, rotationAmount));
}

// === CONSTANT BUFFER ===
void Camera::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	using namespace DirectX;
	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
	m_cameraBuffer->Update(context, &viewProjMatrix);
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
	return m_DH;
}

DX::XMFLOAT3 Camera::GetForward() const
{
	return m_transform.GetRotationF3();
}

DX::XMFLOAT3 Camera::GetPosition() const
{
    return m_transform.GetPositionF3();
}

DX::XMFLOAT3 Camera::GetRight() const
{
	DX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, DirectX::XMVector3Cross(m_transform.GetRotation(), m_up));
	return right;
}

DX::XMFLOAT3 Camera::GetUp() const
{
	DX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, m_up);
	return up;
}
