#include "Camera.h"
#include "MathFuncs.h"

void Camera::MoveInDirection(float amount, const DirectX::XMFLOAT3& direction)
{
	m_position.x += direction.x * amount;
	m_position.y += direction.y * amount;
	m_position.z += direction.z * amount;
}

void Camera::RotateAroundAxis(float amount, const DirectX::XMFLOAT3& axis)
{
	// Implement rotation logic here
}

DX::XMFLOAT4X4& Camera::GenerateViewProjMatrix()
{
	using namespace DirectX;
	XMFLOAT4X4 viewMatrix, projMatrix, viewProjMatrix;

	CreateViewMatrix(viewMatrix, m_position, m_forward, m_up);
	CreateProjectionMatrix(projMatrix, m_projData.fovInDeg, m_projData.aspectRatio, m_projData.nearPlane, m_projData.m_farPlane);
	CreateViewProjMatrix(viewProjMatrix, viewMatrix, projMatrix);
}

Camera::Camera(ID3D11Device* device, ProjectionData& projData, const DX::XMFLOAT3& initialPosition)
    : m_position(initialPosition), m_projData(projData)
{
	using namespace DirectX;

	XMFLOAT4X4 viewProjMatrix = GenerateViewProjMatrix();
    m_cameraBuffer = new ConstantBuffer(device, sizeof(DX::XMFLOAT4X4), &viewProjMatrix);
}

Camera::~Camera()
{
    delete m_cameraBuffer;
}

// === MOVEMENT ===
void Camera::MoveForward(float amount)
{
	MoveInDirection(amount, m_forward);
}

void Camera::MoveRight(float amount)
{
	MoveInDirection(amount, m_right);
}

void Camera::RotateRight(float amount)
{
	RotateAroundAxis(amount, m_up);
}

// === CONSTANT BUFFER ===
void Camera::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	using namespace DirectX;
	XMFLOAT4X4 viewProjMatrix = GenerateViewProjMatrix();
	m_cameraBuffer->Update(context, &viewProjMatrix);
}

// === GETTERS ===
ID3D11Buffer* Camera::GetConstantBuffer() const
{
    return m_cameraBuffer->GetBuffer();
}

const DX::XMFLOAT3& Camera::GetPosition() const
{
    return m_position;
}

const DX::XMFLOAT3& Camera::GetForward() const
{
    return m_forward;
}

const DX::XMFLOAT3& Camera::GetRight() const
{
    return m_right;
}

const DX::XMFLOAT3& Camera::GetUp() const
{
    return m_up;
}
