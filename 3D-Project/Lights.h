#pragma once

class DirLights
{
	public:
	struct DirLight
	{
		
	};
	static const int MAX_DIR_LIGHTS = 3;
	struct DirLightBuffer
	{
		DirLight dirLights[MAX_DIR_LIGHTS];
		size_t numLights;
		float padding[3]; // Padding to ensure 16-byte alignment
	};
};

class SpotLights
{
	public:
	struct SpotLight
	{
		
	};
	static const int MAX_SPOT_LIGHTS = 5;
	struct SpotLightBuffer
	{
		SpotLight spotLights[MAX_SPOT_LIGHTS];
		size_t numLights;
		float padding[3]; // Padding to ensure 16-byte alignment
	};
};


