// Sampler state for texture sampling (from Renderer)
SamplerState samplerState : register(s0);

// Textures coming from the submesh draw call (from SubMesh)
Texture2D ambientTexture : register(t0);
Texture2D diffuseTexture : register(t1);
Texture2D specularTexture : register(t2);

// Material properties (from SubMesh)
cbuffer materialBuffer : register(b0)
{
    int hasAmbientTexture;
    float3 ambientColor;
    
    int hasDiffuseTexture;
    float3 diffuseColor;
    
    int hasSpecularTexture;
    float3 specularColor;
    
    float shininess;
    float3 padding;
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

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;

    // Position: store world-space position (w = 1)
    output.position = float4(input.WORLD_POSITION.xyz, 1.0f);

    // Normal: store normalized world-space normal (w = 0)
    output.normal = float4(normalize(input.NORMAL.xyz), 0.0f);
    
    // DEBUG: Uncomment to visualize UV coordinates as colors (R=U, G=V, B=0)
    // This helps diagnose UV mapping issues - UVs should range from 0-1
    // output.ambient = float4(input.UV.x, input.UV.y, 0.0f, 1.0f);
    // output.diffuse = float4(input.UV.x, input.UV.y, 0.0f, 1.0f);
    // output.specular = float4(0, 0, 0, 1.0f);
    // return output;
    
    // Sample material textures or fallback to constant colors
    float4 ambientSample = (hasAmbientTexture != 0) ? ambientTexture.Sample(samplerState, input.UV)  : float4(ambientColor, 1.0f);
    float4 diffuseSample = (hasDiffuseTexture != 0) ? diffuseTexture.Sample(samplerState, input.UV)  : float4(diffuseColor, 1.0f);
    float4 specularSample = (hasSpecularTexture != 0) ? specularTexture.Sample(samplerState, input.UV) : float4(specularColor, 1.0f);

    // Ensure alpha is valid (use sampled alpha if present, otherwise 1.0)
    ambientSample.a = (ambientSample.a > 0.0f) ? ambientSample.a : 1.0f;
    diffuseSample.a = (diffuseSample.a > 0.0f) ? diffuseSample.a : 1.0f;
    specularSample.a = (specularSample.a > 0.0f) ? specularSample.a : 1.0f;

    output.ambient = ambientSample;
    output.diffuse = diffuseSample;

    // Store specular color in rgb. Put 'shininess' into alpha channel
    output.specular = float4(specularSample.rgb, shininess);

    return output;
}