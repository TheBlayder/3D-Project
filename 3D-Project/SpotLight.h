#pragma once

#include <DirectXMath.h>

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
public:
	SpotLight(const DX::XMFLOAT3& position, const DX::XMFLOAT4& color, const DX::XMFLOAT3& direction, 
		float intensity, float innerConeInDeg, float outerConeInDeg, float range)
		: m_position(position), m_color(color), m_direction(direction), 
		m_intensity(intensity), m_innerConeInDeg(innerConeInDeg), m_outerConeInDeg(outerConeInDeg), m_range(range), padding(0.f, 0.f)
	{}
	~SpotLight() = default;

	DX::XMFLOAT3 GetPosition() const { return m_position; }
	float GetIntensity() const { return m_intensity; }
	DX::XMFLOAT4 GetColor() const { return m_color; }
	DX::XMFLOAT3 GetDirection() const { return m_direction; }
	float GetInnerConeInDeg() const { return m_innerConeInDeg; }
	float GetOuterConeInDeg() const { return m_outerConeInDeg; }
	float GetRange() const { return m_range; }
};