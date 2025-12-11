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

struct PSOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;
};

PSOutput main(PSInput input)
{
    PSOutput output;

    // Position: store world-space position (w = 1)
    output.position = float4(input.WORLD_POSITION.xyz, 1.0f);

    // Normal: normalize and pack from [-1,1] -> [0,1] so it can be stored in an UNORM render target.
    // Unpack in lighting pass with: normal = normalize(normal_rgb * 2.0 - 1.0);
    float3 n = normalize(input.NORMAL.xyz);
    output.normal = float4(n * 0.5f + 0.5f, 1.0f);

    // Sample material textures or fallback to constant colors
    float4 ambientSample = (hasAmbientTexture != 0) ? ambientTexture.Sample(samplerState, input.UV) : float4(ambientColor, 1.0f);
    float4 diffuseSample = (hasDiffuseTexture != 0) ? diffuseTexture.Sample(samplerState, input.UV) : float4(diffuseColor, 1.0f);
    float4 specularSample = (hasSpecularTexture != 0) ? specularTexture.Sample(samplerState, input.UV) : float4(specularColor, 1.0f);

    // Ensure alpha is valid (use sampled alpha if present, otherwise 1.0)
    ambientSample.a = (ambientSample.a > 0.0f) ? ambientSample.a : 1.0f;
    diffuseSample.a = (diffuseSample.a > 0.0f) ? diffuseSample.a : 1.0f;
    specularSample.a = (specularSample.a > 0.0f) ? specularSample.a : 1.0f;

    output.ambient = ambientSample;
    output.diffuse = diffuseSample;

    // Store specular color in rgb. Put 'shininess' (specular power) into alpha so the lighting pass has access to it.
    // If your specular texture's alpha already encodes shininess, adapt here.
    output.specular = float4(specularSample.rgb, shininess);

    return output;
}