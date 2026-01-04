SamplerState samplerState : register(s0);

TextureCube cubeMap : register(t0);
Texture2D ambientTexture : register(t1);
Texture2D diffuseTexture : register(t2);
Texture2D specularTexture : register(t3);

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

// Camera properties (from DCEM)
cbuffer CameraBuffer : register(b1)
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

struct PSOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3; 
    float4 specular : SV_Target4;
};

static const float ambientStrength = 0.3f;

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    output.position = float4(input.WORLD_POSITION.xyz, 1.0f);
    output.normal = float4(normalize(input.NORMAL.xyz), 0.0f);
    
    float3 viewDir = normalize(input.WORLD_POSITION.xyz - cameraPosition.xyz);
    float3 reflectDir = reflect(viewDir, normalize(input.NORMAL.xyz));
    
    output.ambient = cubeMap.Sample(samplerState, reflectDir) * ambientStrength;
    output.diffuse = diffuseTexture.Sample(samplerState, input.UV);
    output.specular = hasSpecularTexture ? specularTexture.Sample(samplerState, input.UV) : float4(0, 0, 0, 1);

    return output;
}