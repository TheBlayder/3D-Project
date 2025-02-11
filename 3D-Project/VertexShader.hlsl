
struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 normal : TEXCOORD1;
    //float4 worldPosition : WORLD_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f);
    output.normal = float4(input.normal, 1.0f);
    //output.worldPosition = float4(input.position, 1.0f);
    output.uv = input.uv;
    return output;
}