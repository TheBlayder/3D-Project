#pragma once

#include <DirectXMath.h>
#include "Transform.h"

namespace DX = DirectX;

namespace MatrixHelper
{
    inline void CreateWorldMatrix(DX::XMFLOAT4X4& worldMatrix, const Transform& transform)  
    {  
        DX::XMMATRIX scaling = DX::XMMatrixScalingFromVector(transform.GetScale());
   		DX::XMMATRIX rotation = DX::XMMatrixRotationRollPitchYawFromVector(transform.GetRotation());
		DX::XMMATRIX translation = DX::XMMatrixTranslationFromVector(transform.GetPosition());
        
        DX::XMMATRIX world = DX::XMMatrixMultiplyTranspose(DX::XMMatrixMultiply(scaling, rotation), translation);

        DX::XMStoreFloat4x4(&worldMatrix, world);
    }

    inline void CreateViewMatrix(DX::XMFLOAT4X4& viewMatrix, const DX::XMVECTOR& position, const DX::XMVECTOR& forward, const DX::XMVECTOR& up)
    {
        DX::XMMATRIX view = DX::XMMatrixLookAtLH(position, forward, up);
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
        DX::XMMATRIX viewProj = DX::XMMatrixMultiplyTranspose(view, proj);

        DX::XMStoreFloat4x4(&viewProjMatrix, viewProj);
    }
}
