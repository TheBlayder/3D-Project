// Standard Sampler (from Renderer)
SamplerState samplerState : register(s0);

// Camera properties (from DCEM)
cbuffer CameraBuffer : register(b0)
{
    float4 cameraPosition;
};

// Textures coming from the submesh draw call (from SubMesh)
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

TextureCube cubeMap : register(t3);

struct PSInput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : WORLD_POSITION;
    float4 NORMAL : NORMAL;
    float2 UV : UV;
};

struct PSOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;
};

//float4 main(PSInput input) : SV_TARGET
//{
//    float3 incomingView = normalize(input.WORLD_POSITION.xyz - cameraPosition.xyz);
//    float3 reflectedView = reflect(incomingView, normalize(input.NORMAL.xyz));

//    float4 output = cubeMap.Sample(samplerState, reflectedView);
//    float4 output = 1;
    
//    return output;
//}

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    output.position = input.WORLD_POSITION;
    output.normal = input.NORMAL;
    
    float3 incomingView = normalize(input.WORLD_POSITION.xyz - cameraPosition.xyz);
    float3 reflectedView = reflect(incomingView, normalize(input.NORMAL.xyz));
    output.ambient = cubeMap.Sample(samplerState, reflectedView);
    output.diffuse = 0;
    output.specular = 0;
    
    return output;
}