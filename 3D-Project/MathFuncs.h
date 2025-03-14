#pragma once

#include <DirectXMath.h>

namespace DX = DirectX;

void CreateWorldMatrix(DX::XMFLOAT4X4& worldMatrix, const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float rotationInDeg)
{
    using namespace DirectX;
    XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
    XMMATRIX rotation = XMMatrixRotationY(XMConvertToRadians(rotationInDeg));
    XMMATRIX world = XMMatrixMultiplyTranspose(XMMatrixMultiply(scaling, translation), rotation);

    XMStoreFloat4x4(&worldMatrix, world);
}

void CreateViewMatrix(DX::XMFLOAT4X4& viewMatrix, const DX::XMFLOAT3& position, const DX::XMFLOAT3& forward, const DX::XMFLOAT3& up)
{
    using namespace DirectX;
    XMVECTOR eyePos = XMLoadFloat3(&position);
    XMVECTOR directionVec = XMVectorAdd(eyePos, XMLoadFloat3(&forward));
    XMVECTOR upVec = XMLoadFloat3(&up);
    XMMATRIX view = XMMatrixLookAtLH(eyePos, directionVec, upVec);

    XMStoreFloat4x4(&viewMatrix, view);
}

void CreateProjectionMatrix(DX::XMFLOAT4X4& projMatrix, const float fovInDeg, const float aspectRatio, const float nearPlane, const float farPlane)
{
    using namespace DirectX;
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovInDeg), aspectRatio, nearPlane, farPlane);

    XMStoreFloat4x4(&projMatrix, proj);
}

void CreateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix, const DX::XMFLOAT4X4& viewMatrix, const DX::XMFLOAT4X4& projMatrix)
{
    using namespace DirectX;
    XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
    XMMATRIX proj = XMLoadFloat4x4(&projMatrix);
    XMMATRIX viewProj = XMMatrixMultiplyTranspose(view, proj);

    XMStoreFloat4x4(&viewProjMatrix, viewProj);
}