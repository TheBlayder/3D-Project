cbuffer ViewProjectionBuffer : register(b0)
{
    float4x4 viewProjMatrix;
};

cbuffer WorldMatrix : register(b1)
{
    float4x4 worldMatrix;
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
    output.WORLD_POSITION = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.WORLD_POSITION, viewProjMatrix);
    output.NORMAL = normalize(float4(mul(float4(input.normal, 0), worldMatrix).xyz, 0));
    output.UV = input.uv;
    return output;
}