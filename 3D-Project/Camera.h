#pragma once
#include "d3d11.h"
#include "DirectXMath.h"

namespace DX = DirectX;
class Camera
{
private:
	DX::XMFLOAT4 m_position;
	DX::XMFLOAT4 m_direction; // Make sure this is normalized

	DX::XMFLOAT4 m_focusPosition;
	DX::XMFLOAT4 m_up;

	float m_fovInDeg;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	DX::XMMATRIX* m_viewMatrix;
	DX::XMMATRIX* m_projMatrix;

	void CreateViewMatrix(DX::XMMATRIX*& viewMatrix, const DX::XMFLOAT4& pos, const DX::XMFLOAT4& focusPos, DX::XMFLOAT4& up);
	void CreateProjMatrix(DX::XMMATRIX*& projMatrix, const float& fovInDeg, const float& aspectRatio, const float& nearPlane, const float& farPlane);

	void updateViewMatrix();
public:
	Camera(DX::XMFLOAT4& posistion, DX::XMFLOAT4& direction);
	~Camera();

	void SetPosition(const DX::XMFLOAT4& position);
	void SetDirection(const DX::XMFLOAT4& direction);

	const DX::XMFLOAT4& GetPosition() const;
	const DX::XMFLOAT4& GetDirection() const;

	const DX::XMMATRIX* GetViewMatrix() const;
	const DX::XMMATRIX* GetProjMatrix() const;
};

