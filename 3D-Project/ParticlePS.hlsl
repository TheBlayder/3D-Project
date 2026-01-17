
struct PSInput
{
    float4 position : SV_POSITION;
    float4 NORMAL : NORMAL;
    float4 color : COLOR;
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

const float ambientStrength = 0.3f;
PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    output.position = input.position;
    output.normal = input.NORMAL;
    
    
    // Simple circle/sphere shape for particle
    float2 centeredUV = input.UV * 2.0f - 1.0f;
    float dist = length(centeredUV);
    
    if (dist > 1.0f)
        discard;
    
    // Soft edge falloff
    float alpha = 1.0f - smoothstep(0.8f, 1.0f, dist);

    output.ambient = float4(input.color.rgb, input.color.a * alpha);
    output.diffuse = input.color;
    output.specular = input.color;
    
    return output;
}