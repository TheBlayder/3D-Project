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