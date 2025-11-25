SamplerState samplerState : register(s0);

// Textures coming from the submesh draw call
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

cbuffer materialBuffer : register(b0)
{
    int hasAmbientTexture;
    float3 ambientColor;
    
    int hasDiffuseTexture;
    float3 diffuseColor;
    
    int hasSpecularTexture;
    float3 specularColor;
    
    float shininess;
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
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}