#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Transform.h"

namespace DX = DirectX;

struct LightData
{
	float position[3];
	float intensity;
	float color[4];
	float direction[3];
	float padding;
};

class LightHandler
{
private:
	std::vector<SpotLight> m_spotLights;
	std::vector<DirectionalLight> m_directionalLights;

public:
	LightHandler() = default;
	~LightHandler() = default;
	
	void Init(ID3D11Device* device);

	// SpotLight
	void AddSpotLight(const SpotLight& spotLight);
	const std::vector<SpotLight>& GetSpotLights() const;

	// DirectionalLight
	void AddDirectionalLight(const DirectionalLight& directionalLight);
	const std::vector<DirectionalLight>& GetDirectionalLights() const;
};

