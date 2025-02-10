struct PSInput
{
    float4 position : SV_POSITION;
    float4 normal : TEXCOORD1;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    // Use input.normal as needed
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}