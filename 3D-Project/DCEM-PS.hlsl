// Standard Sampler (from Renderer)
SamplerState samplerState : register(s0);

// Cube map (from DCEM)
TextureCube cubeMap : register(t0);

// Camera properties (from DCEM)
cbuffer CameraBuffer : register(b0)
{
    float4 cameraPosition;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : WORLD_POSITION;
    float4 NORMAL : NORMAL;
    float2 UV : UV;
};

float4 main(PSInput input) : SV_TARGET
{
    float3 incomingView = normalize(input.WORLD_POSITION.xyz - cameraPosition.xyz);
    float3 reflectedView = reflect(incomingView, normalize(input.NORMAL.xyz));

    float4 output = cubeMap.Sample(samplerState, reflectedView);
    
    return output;
}