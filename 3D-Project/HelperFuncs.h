#pragma once

#include <DirectXMath.h>
#include "Transform.h"

namespace DX = DirectX;

namespace MatrixHelper
{
    inline void CreateWorldMatrix(DX::XMFLOAT4X4& worldMatrix, const Transform& transform)
    {
        // Build S, R, T correctly
        DX::XMMATRIX scaling = DX::XMMatrixScaling(transform.GetScaleF3().x, transform.GetScaleF3().y, transform.GetScaleF3().z);
        DX::XMMATRIX translation = DX::XMMatrixTranslation(transform.GetPositionF3().x, transform.GetPositionF3().y, transform.GetPositionF3().z);

        DX::XMMATRIX rotationX = DX::XMMatrixRotationX((DX::XMConvertToRadians(transform.GetRotationF3().x)));
        DX::XMMATRIX rotationY = DX::XMMatrixRotationY((DX::XMConvertToRadians(transform.GetRotationF3().y)));
        DX::XMMATRIX rotationZ = DX::XMMatrixRotationZ((DX::XMConvertToRadians(transform.GetRotationF3().z)));
        DX::XMMATRIX rotation = DX::XMMatrixMultiply(DX::XMMatrixMultiply(rotationZ, rotationY), rotationX);

        DX::XMMATRIX world = DX::XMMatrixMultiplyTranspose(DX::XMMatrixMultiply(scaling, rotation), translation);

        DX::XMStoreFloat4x4(&worldMatrix, world);
    }

    inline void CreateViewMatrix(DX::XMFLOAT4X4& viewMatrix, const DX::XMVECTOR& position, const DX::XMVECTOR& forward, const DX::XMVECTOR& up)
    {
        // If 'forward' is a direction vector, the "at" point is eye + forward
        DX::XMVECTOR at = DX::XMVectorAdd(position, forward);
        DX::XMMATRIX view = DX::XMMatrixLookAtLH(position, at, up);
        DX::XMStoreFloat4x4(&viewMatrix, view);
    }

    inline void CreateProjectionMatrix(DX::XMFLOAT4X4& projMatrix, const float fovInDeg, const float aspectRatio, const float nearPlane, const float farPlane)
    {
        DX::XMMATRIX proj = DX::XMMatrixPerspectiveFovLH(DX::XMConvertToRadians(fovInDeg), aspectRatio, nearPlane, farPlane);
        DX::XMStoreFloat4x4(&projMatrix, proj);
    }

    inline void CreateViewProjMatrix(DX::XMFLOAT4X4& viewProjMatrix, const DX::XMFLOAT4X4& viewMatrix, const DX::XMFLOAT4X4& projMatrix)
    {
        DX::XMMATRIX view = DX::XMLoadFloat4x4(&viewMatrix);
        DX::XMMATRIX proj = DX::XMLoadFloat4x4(&projMatrix);
        DX::XMMATRIX viewProj = DX::XMMatrixMultiply(view, proj);
        DX::XMStoreFloat4x4(&viewProjMatrix, viewProj);
    }
}
