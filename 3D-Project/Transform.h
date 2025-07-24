#pragma once

#include <DirectXMath.h>
namespace DX = DirectX;

class Transform
{
public:
	inline Transform();
	inline Transform(const DX::XMVECTOR& position, const DX::XMVECTOR& rotation, const DX::XMVECTOR& scale);

	inline void SetPosition(const DX::XMVECTOR& position);
	inline void SetRotation(const DX::XMVECTOR& rotation);
	inline void SetScale(const DX::XMVECTOR& scale);

	inline DX::XMVECTOR GetPosition() const;
	inline DX::XMVECTOR GetRotation() const;
	inline DX::XMVECTOR GetScale() const;

	inline DX::XMFLOAT3 GetPositionF3() const;
	inline DX::XMFLOAT3 GetRotationF3() const;
	inline DX::XMFLOAT3 GetScaleF3() const;

private:
	DX::XMVECTOR m_position; 
	DX::XMVECTOR m_rotation;
	DX::XMVECTOR m_scale;    
};

inline Transform::Transform()
	: m_position(DX::XMVectorZero()), m_rotation(DX::XMVectorZero()), m_scale({1,1,1}) {}

inline Transform::Transform(const DX::XMVECTOR& position, const DX::XMVECTOR& rotation, const DX::XMVECTOR& scale) 
	: m_position(position), m_rotation(rotation), m_scale(scale) {}

inline void Transform::SetPosition(const DX::XMVECTOR& position) { m_position = position; }

inline void Transform::SetRotation(const DX::XMVECTOR& rotation) { m_rotation = rotation; }

inline void Transform::SetScale(const DX::XMVECTOR& scale) { m_scale = scale; }

inline DX::XMVECTOR Transform::GetPosition() const { return m_position; }

inline DX::XMVECTOR Transform::GetRotation() const { return m_rotation; }

inline DX::XMVECTOR Transform::GetScale() const { return m_scale; }

inline DX::XMFLOAT3 Transform::GetPositionF3() const
{
	DX::XMFLOAT3 pos;
	DX::XMStoreFloat3(&pos, m_position);
	return pos;
}

inline DX::XMFLOAT3 Transform::GetRotationF3() const
{
	DX::XMFLOAT3 rot;
	DX::XMStoreFloat3(&rot, m_rotation);
	return rot;
}

inline DX::XMFLOAT3 Transform::GetScaleF3() const
{
	DX::XMFLOAT3 scale;
	DX::XMStoreFloat3(&scale, m_scale);
	return scale;
}


