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
    // Use ternary checks on the int flags to decide between sampling or a default color
    float4 ambientSample = (hasAmbientTexture != 0)  ? ambientTexture.Sample(samplerState, input.UV) : float4(ambientColor, 1.0f);
    float4 diffuseSample = (hasDiffuseTexture != 0)  ? diffuseTexture.Sample(samplerState, input.UV) : float4(diffuseColor, 1.0f);
    float4 specularSample = (hasSpecularTexture != 0) ? specularTexture.Sample(samplerState, input.UV) : float4(specularColor, 1.0f);

    float4 combined = (ambientSample + diffuseSample + specularSample);

    combined.a = 1.0f;

    return combined;
}