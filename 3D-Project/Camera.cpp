#include "Camera.h"

Camera::Camera(DX::XMFLOAT4& position, DX::XMFLOAT4& direction)
    : m_position(position), m_direction(direction), m_up(DX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f))
{
    m_viewMatrix = new DX::XMMATRIX();
    m_projMatrix = new DX::XMMATRIX();
	CreateViewMatrix(m_viewMatrix, position, DX::XMFLOAT4(0.f,0.f,0.f,1.f), m_up);
    CreateProjMatrix(m_projMatrix, m_fovInDeg, m_aspectRatio, m_nearPlane, m_farPlane);
}

Camera::~Camera()
{
    delete m_viewMatrix;
    delete m_projMatrix;
}

void Camera::CreateViewMatrix(DX::XMMATRIX*& viewMatrix, const DX::XMFLOAT4& pos, const DX::XMFLOAT4& focusPos, DX::XMFLOAT4& up)
{
    DX::XMVECTOR posVec = DX::XMLoadFloat4(&pos);
    DX::XMVECTOR focusPosVec = DX::XMLoadFloat4(&focusPos);
    DX::XMVECTOR upVec = DX::XMLoadFloat4(&up);
    *viewMatrix = DX::XMMatrixLookAtLH(posVec, focusPosVec, upVec);
}

void Camera::CreateProjMatrix(DX::XMMATRIX*& projMatrix, const float& fovInDeg, const float& aspectRatio, const float& nearPlane, const float& farPlane)
{
    *projMatrix = DX::XMMatrixPerspectiveFovLH(DX::XMConvertToRadians(fovInDeg), aspectRatio, nearPlane, farPlane);
}

void Camera::updateViewMatrix()
{
    DX::XMVECTOR posVec = DX::XMLoadFloat4(&m_position);
    DX::XMVECTOR dirVec = DX::XMLoadFloat4(&m_direction);
    DX::XMVECTOR focusPosVec = DX::XMVectorAdd(posVec, dirVec);
    DX::XMVECTOR upVec = DX::XMLoadFloat4(&m_up);
    *m_viewMatrix = DX::XMMatrixLookAtLH(posVec, focusPosVec, upVec);
}

// === GETTERS AND SETTERS ===

void Camera::SetPosition(const DX::XMFLOAT4& pos)
{
    m_position = pos;
    updateViewMatrix();
}

void Camera::SetDirection(const DX::XMFLOAT4& dir)
{
    m_direction = dir;
    updateViewMatrix();
}

const DX::XMFLOAT4& Camera::GetPosition() const
{
    return m_position;
}

const DX::XMFLOAT4& Camera::GetDirection() const
{
    return m_direction;
}

const DX::XMMATRIX* Camera::GetViewMatrix() const
{
    return m_viewMatrix;
}

const DX::XMMATRIX* Camera::GetProjMatrix() const
{
    return m_projMatrix;
}