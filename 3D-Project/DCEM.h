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

using namespace Microsoft;
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

class DCEM {
private:
	std::array<Camera, 6> m_cameras;
	std::array<WRL::ComPtr<ID3D11RenderTargetView>, 6> m_cubeMapRTVs;
	WRL::ComPtr<ID3D11Texture2D> m_cubeMapTex;
	WRL::ComPtr<ID3D11ShaderResourceView> m_cubeMapSRV;

	WRL::ComPtr<ID3D11Texture2D> m_depthTex;
	std::array<WRL::ComPtr<ID3D11DepthStencilView>, 6> m_DSVs;

	D3D11_VIEWPORT m_viewport;
	
	Transform m_transform;
	Mesh m_mesh;
	UINT m_resolution;

	ConstantBuffer m_cameraBuffer;

	void Init(ID3D11Device* device);

	void Render(ID3D11DeviceContext* context, const std::vector<BaseObject*>& sceneObjects, ConstantBuffer* worldBuffer, ConstantBuffer* viewProjBuffer, const size_t face);
	void Draw(ID3D11DeviceContext* context, ConstantBuffer* worldBuffer, ConstantBuffer* viewProjBuffer, Camera* camera);

public:
	DCEM(ID3D11Device* device, const Transform& transform, const UINT& resolution, std::string& folderPath, std::string& objectName);

	void RenderAndDraw(ID3D11DeviceContext* context, const std::vector<BaseObject*>& sceneObjects, ConstantBuffer* worldBuffer, 
		ConstantBuffer* viewProjBuffer, Camera* camera, ID3D11PixelShader* dcemPS, ID3D11PixelShader* returnPS, D3D11_VIEWPORT* returnVP);

	const DirectX::XMFLOAT4X4 GetWorldMatrix();
};