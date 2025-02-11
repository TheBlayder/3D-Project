

struct VertexShaderInput
{
    float3 position;
    float3 normal;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = float4(input.position, 1.0f);
    output.normal = input.normal;
    return output;
}