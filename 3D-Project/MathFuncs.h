#pragma once

#include <DirectXMath.h>

namespace DX = DirectX;

DX::XMFLOAT4X4& CreateWorldMatrix(const DX::XMFLOAT3& scale, const DX::XMFLOAT3& position, const float rotationInDeg)
{
	using namespace DirectX;
	XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotation = XMMatrixRotationY(XMConvertToRadians(rotationInDeg));
	XMMATRIX world = XMMatrixMultiplyTranspose(XMMatrixMultiply(scaling, translation), rotation);

	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, world);
	return worldMatrix;
}

DX::XMFLOAT4X4& CreateViewMatrix(const DX::XMFLOAT3& position, const DX::XMFLOAT3& forward, const DX::XMFLOAT3& up)
{
	using namespace DirectX;
	XMVECTOR eyePos = XMLoadFloat3(&position);
	XMVECTOR directionVec = XMVectorAdd(eyePos, XMLoadFloat3(&forward));
	XMVECTOR upVec = XMLoadFloat3(&up);
	XMMATRIX view = XMMatrixLookAtLH(eyePos, directionVec, upVec);

	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, view);
	return viewMatrix;
}

DX::XMFLOAT4X4& CreateProjectionMatrix(const float fovInDeg, const float aspectRatio, const float nearPlane, const float farPlane)
{
	using namespace DirectX;
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovInDeg), aspectRatio, nearPlane, farPlane);

	XMFLOAT4X4 projMatrix;
	XMStoreFloat4x4(&projMatrix, proj);
	return projMatrix;
}

DX::XMFLOAT4X4& CreateViewProjMatrix(const DX::XMFLOAT4X4& viewMatrix, const DX::XMFLOAT4X4& projMatrix)
{
	using namespace DirectX;
	XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&projMatrix);
	XMMATRIX viewProj = XMMatrixMultiplyTranspose(view, proj);

	XMFLOAT4X4 viewProjMatrix;
	XMStoreFloat4x4(&viewProjMatrix, viewProj);
	return viewProjMatrix;
}