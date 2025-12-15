#include "Camera.h"
#include "HelperFuncs.h"

namespace MH = MatrixHelper;

void Camera::MoveInDirection(float amount, const DirectX::XMFLOAT3& direction)
{
	m_transform.SetPosition(DirectX::XMVectorAdd(m_transform.GetPosition(), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), amount)));
}

void Camera::RotateAroundAxis(float amount) // Will only rotate around the up axis for now
{
	using namespace DirectX;
	XMVECTOR currentRotation = m_transform.GetRotation();
	XMVECTOR rotationAmount = XMVectorScale(m_up, amount);
	m_transform.SetRotation(XMVectorAdd(currentRotation, rotationAmount));
}

void Camera::GenerateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix)
{
	using namespace DirectX;
	XMFLOAT4X4 viewMatrix, projMatrix;

	MH::CreateViewMatrix(viewMatrix, m_transform.GetPosition(), m_transform.GetRotation(), m_up);
	MH::CreateProjectionMatrix(projMatrix, m_projData.fovInDeg, m_projData.aspectRatio, m_projData.nearPlane, m_projData.m_farPlane);
	MH::CreateViewProjMatrix(viewProjMatrix, viewMatrix, projMatrix);
}

Camera::Camera(ID3D11Device* device, ProjectionData& projData, const DX::XMFLOAT3& initialPosition)
    : m_projData(projData)
{
	using namespace DirectX;
	m_transform.SetPosition(XMLoadFloat3(&initialPosition));
	m_transform.SetRotation(XMVectorSet(0.f, 0.f, 0.001f, 0.f));

	XMFLOAT4X4 viewProjMatrix;
	GenerateViewProjMatrix(viewProjMatrix);
    m_cameraBuffer = new ConstantBuffer(device, sizeof(DX::XMFLOAT4X4), &viewProjMatrix);

}

Camera::~Camera()
{
	delete m_cameraBuffer;
	delete m_DH;
}

// === MOVEMENT ===
void Camera::MoveForward(float amount)
{
	MoveInDirection(amount, m_transform.GetRotationF3());
}

void Camera::RotateRight(float amount)
{
	RotateAroundAxis(amount);
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
const DirectX::XMFLOAT4X4 Camera::GetViewProjMatrix()
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

const DX::XMFLOAT3& Camera::GetForward() const
{
	return m_transform.GetRotationF3();
}

const DX::XMFLOAT3& Camera::GetPosition() const
{
    return m_transform.GetPositionF3();
}

const DX::XMFLOAT3& Camera::GetRight() const
{
	DX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, DirectX::XMVector3Cross(m_transform.GetPosition(), m_up));
	return right;
}

const DX::XMFLOAT3& Camera::GetUp() const
{
	DX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, m_up);
	
	return up;
}
