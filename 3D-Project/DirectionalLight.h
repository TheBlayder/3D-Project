#pragma once

#include <DirectXMath.h>
#include <wrl/client.h>

#include "HelperFuncs.h"

namespace DX = DirectX;

class DirectionalLight
{
private:
	DX::XMFLOAT4 m_color;
	DX::XMFLOAT3 m_direction;
	float m_intensity; // Fixed typo here

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
public:
	inline DirectionalLight(const DX::XMFLOAT4& color, const DX::XMFLOAT3& direction, float intensity)
		: m_color(color), m_direction(direction), m_intensity(intensity) // And here
	{}
	~DirectionalLight() = default;

	inline HRESULT Init(ID3D11Device* device, ID3D11Texture2D* depthStencil, D3D11_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		return device->CreateDepthStencilView(depthStencil, desc, DSV.GetAddressOf());
	}

	inline DX::XMFLOAT4 GetColor() const { return m_color; }
	inline DX::XMFLOAT3 GetDirection() const { return m_direction; }
	inline float GetIntensity() const { return m_intensity; } // And here
	inline ID3D11DepthStencilView* GetDSV() const { return DSV.Get(); }

	inline DX::XMFLOAT4X4 GetViewProjMatrix() const
	{
		using namespace DirectX;
		XMFLOAT4X4 view{};
		XMFLOAT4X4 proj{};
		XMFLOAT4X4 viewProj{};

		// Normalize light direction
		XMVECTOR dir_n = XMVector3Normalize(XMLoadFloat3(&m_direction));

		// Compute a light position far along the direction so the orthographic frustum covers the scene
		XMVECTOR sceneCenter = XMVectorZero();
		float lightDistance = 100.f;
		XMVECTOR lightPos = XMVectorSubtract(sceneCenter, XMVectorScale(dir_n, lightDistance));

		// Choose an up vector that is not colinear with the light direction.
		// If the light direction is (nearly) parallel to (0,1,0), using (0,1,0) as up will produce a degenerate view matrix.
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR dot = XMVector3Dot(dir_n, up);
		float dotVal = fabsf(XMVectorGetX(dot));
		if (dotVal > 0.999f) // nearly parallel -> pick a different up
		{
			up = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		}

		MatrixHelper::CreateViewMatrix(view, lightPos, sceneCenter, up);

		const float orthoSize = 100.f;
		const float nearZ = 0.1f;
		const float farZ = 200.f;

		MatrixHelper::CreateOrthographicMatrix(proj, orthoSize, orthoSize, nearZ, farZ);
		MatrixHelper::CreateViewProjMatrix(viewProj, view, proj);

		return viewProj;
	}
};