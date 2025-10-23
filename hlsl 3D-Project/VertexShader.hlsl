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
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : TEXCOORD2;
    float4 NORMAL : TEXCOORD1;
    float2 UV : TEXCOORD0;
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