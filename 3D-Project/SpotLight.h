#pragma once

#include <DirectXMath.h>
#include <wrl/client.h>

#include "HelperFuncs.h"

namespace DX = DirectX;

class SpotLight
{
private:
	DX::XMFLOAT3 m_position;
	float m_intensity;
	DX::XMFLOAT4 m_color;
	DX::XMFLOAT3 m_direction;
	float m_innerConeInDeg;
	float m_outerConeInDeg;
	float m_range;
	DX::XMFLOAT2 padding = {0.f, 0.f};

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
public:
	inline SpotLight(const DX::XMFLOAT3& position, const DX::XMFLOAT4& color, const DX::XMFLOAT3& direction, 
		float intensity, float innerConeInDeg, float outerConeInDeg, float range)
		: m_position(position), m_color(color), 
		m_intensity(intensity), m_innerConeInDeg(innerConeInDeg), m_outerConeInDeg(outerConeInDeg), m_range(range), padding(0.f, 0.f)
	{
		// Normalize direction on construction
		DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&direction);
		dirVec = DirectX::XMVector3Normalize(dirVec);
		DirectX::XMStoreFloat3(&m_direction, dirVec);
	}
	~SpotLight() = default;

	inline HRESULT Init(ID3D11Device* device, ID3D11Texture2D* depthStencil, D3D11_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		return device->CreateDepthStencilView(depthStencil, desc, DSV.GetAddressOf());
	}

	inline DX::XMFLOAT3 GetPosition() const { return m_position; }
	inline float GetIntensity() const { return m_intensity; }
	inline DX::XMFLOAT4 GetColor() const { return m_color; }
	inline DX::XMFLOAT3 GetDirection() const { return m_direction; }
	inline float GetInnerConeInDeg() const { return m_innerConeInDeg; }
	inline float GetOuterConeInDeg() const { return m_outerConeInDeg; }
	inline float GetRange() const { return m_range; }

	inline ID3D11DepthStencilView* GetDSV() const { return DSV.Get(); }

	inline DX::XMFLOAT4X4 GetViewProjMatrix() const
	{
		using namespace DirectX;

		XMVECTOR pos = XMLoadFloat3(&m_position);
		XMVECTOR dir_n = XMLoadFloat3(&m_direction);

		// Choose an up vector that isn't colinear with the light direction
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		float dotVal = fabsf(XMVectorGetX(XMVector3Dot(dir_n, up)));
		if (dotVal > 0.999f) // nearly parallel
		{
			up = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		}

		XMFLOAT4X4 view{};
		XMFLOAT4X4 proj{};
		XMFLOAT4X4 viewProj{};

		float fovInDeg = m_outerConeInDeg * 2.f;
		float aspectRatio = 1.f; // square shadow map
		float nearZ = 0.5f;
		float farZ = m_range;

		MatrixHelper::CreateViewMatrix(view, pos, dir_n, up);
		MatrixHelper::CreateProjectionMatrix(proj, fovInDeg, aspectRatio, nearZ, farZ);
		MatrixHelper::CreateViewProjMatrix(viewProj, view, proj);

		return viewProj;
	}
};