// Textures coming from the submesh draw call
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

SamplerState samplerState : register(s0);


struct PSInput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : TEXCOORD2;
    float4 NORMAL : TEXCOORD1;
    float2 UV : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}