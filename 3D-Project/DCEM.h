#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <array>
#include <wrl/client.h>
#include <string>

#include "ConstantBuffer.h"	
#include "BaseObject.h"
#include "Camera.h"
#include "Mesh.h"

namespace DX = DirectX;

enum TEXTURE_CUBE_FACES
{
	POS_X = 0,
	NEG_X = 1,
	POS_Y = 2,
	NEG_Y = 3,
	POS_Z = 4,
	NEG_Z = 5
};

class DCEM : public BaseObject
{
private:
	std::array<Camera, 6> m_cameras;
	std::array<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>, 6> m_cubeMapUAVs;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cubeMapTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMapSRV;

	ID3D11PixelShader* dcemPS;
	ID3D11PixelShader* returnPS;

	UINT m_resolution;

	ConstantBuffer m_cameraBuffer;

	void Init(ID3D11Device* device);

public:
	DCEM(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName,
	ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS);
	
	void Draw(ID3D11DeviceContext* context) const override;
	
	void Update(float deltaTime) override {};
	void Update(ID3D11DeviceContext* context, float deltaTime, Camera* cam);

	const void GetWorldMatrix(DX::XMFLOAT4X4& worldMatrix);
	const std::array<Camera, 6>& GetCameras() { return m_cameras; }
	std::array<ID3D11UnorderedAccessView**, 6> GetUAVAdresses();
};