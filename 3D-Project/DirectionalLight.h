#pragma once

#include <DirectXMath.h>

namespace DX = DirectX;

class DirectionalLight
{
private:
	DX::XMFLOAT4 m_color;
	DX::XMFLOAT3 m_direction;
	float inensity;
public:
	DirectionalLight(const DX::XMFLOAT4& color, const DX::XMFLOAT3& direction, float intensity)
		: m_color(color), m_direction(direction), inensity(intensity)
	{}
	~DirectionalLight() = default;

	DX::XMFLOAT4 GetColor() const { return m_color; }
	DX::XMFLOAT3 GetDirection() const { return m_direction; }
	float GetIntensity() const { return inensity; }
};