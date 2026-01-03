#pragma once
#include <d3d11.h>
#include <array>
#include <wrl/client.h>


#include "BaseObject.h"
#include "Camera.h"


enum TEXTURE_CUBE_FACES
{
	POS_X = 0,
	NEG_X = 1,
	POS_Y = 2,
	NEG_Y = 3,
	POS_Z = 4,
	NEG_Z = 5
};

class DCEM : public BaseObject {
private:
	std::array<Camera, 6> m_cameras;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cubeMapTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_cubeMapRTV;

	UINT m_size;

public:
	DCEM(Transform& transform, UINT size);
	void Update(float deltaTime) override;

	void Init(ID3D11Device* device);
	
};