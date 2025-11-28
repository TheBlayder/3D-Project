cbuffer WorldMatrix : register(b0)
{
    float4x4 worldMatrix;
};

cbuffer ViewProjectionMatrix : register(b1)
{
    float4x4 viewProjMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : WORLD_POSITION;
    float4 NORMAL : NORMAL;
    float2 UV : UV;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    float4 modelPos = float4(input.position, 1.0f);
    output.WORLD_POSITION = mul(modelPos, worldMatrix);
    output.position = mul(output.WORLD_POSITION, viewProjMatrix);
    
    output.NORMAL = normalize(float4(mul(float4(input.normal, 1.0f), worldMatrix).xyz, 0.0f));
    output.UV = input.uv;
    return output;
}