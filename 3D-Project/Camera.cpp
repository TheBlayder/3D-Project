#include "Camera.h"

Camera::Camera(DX::XMFLOAT4& position, DX::XMFLOAT4& direction)
    : position(position), direction(direction), up(DX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f))
{
    viewMatrix = new DX::XMMATRIX();
    projMatrix = new DX::XMMATRIX();
	CreateViewMatrix(viewMatrix, position, DX::XMFLOAT4(0.f,0.f,0.f,1.f), up);
    CreateProjMatrix(projMatrix, fovInDeg, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera()
{
    delete viewMatrix;
    delete projMatrix;
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
    DX::XMVECTOR posVec = DX::XMLoadFloat4(&position);
    DX::XMVECTOR dirVec = DX::XMLoadFloat4(&direction);
    DX::XMVECTOR focusPosVec = DX::XMVectorAdd(posVec, dirVec);
    DX::XMVECTOR upVec = DX::XMLoadFloat4(&up);
    *viewMatrix = DX::XMMatrixLookAtLH(posVec, focusPosVec, upVec);
}

// === GETTERS AND SETTERS ===

void Camera::SetPosition(const DX::XMFLOAT4& pos)
{
    position = pos;
    updateViewMatrix();
}

void Camera::SetDirection(const DX::XMFLOAT4& dir)
{
    direction = dir;
    updateViewMatrix();
}

const DX::XMFLOAT4& Camera::GetPosition() const
{
    return position;
}

const DX::XMFLOAT4& Camera::GetDirection() const
{
    return direction;
}

const DX::XMMATRIX* Camera::GetViewMatrix() const
{
    return viewMatrix;
}

const DX::XMMATRIX* Camera::GetProjMatrix() const
{
    return projMatrix;
}