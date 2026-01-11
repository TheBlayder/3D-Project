
cbuffer Camera : register(b0)
{
    float4x4 vpMatrix;
};

struct HSOutput
{
    float3 WORLD_POSITION : WORLD_POSITION;
    float3 NORMAL : NORMAL;
    float2 UV : UV;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : WORLD_POSITION;
    float4 NORMAL : NORMAL;
    float2 UV : UV;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DSOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation,
	const OutputPatch<HSOutput, NUM_CONTROL_POINTS> patch)
{
    
    float3 linearPosition = patch[0].WORLD_POSITION * barycentric.x + patch[1].WORLD_POSITION * barycentric.y + patch[2].WORLD_POSITION * barycentric.z;
    float3 linearNormal = normalize(patch[0].NORMAL * barycentric.x + patch[1].NORMAL * barycentric.y + patch[2].NORMAL * barycentric.z);
    float2 linearUV = patch[0].UV * barycentric.x + patch[1].UV * barycentric.y + patch[2].UV * barycentric.z;
    
    float3 phongPosition = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < NUM_CONTROL_POINTS; ++i)
    {
        float3 pointOnTangentPlane = linearPosition - (dot(linearPosition - patch[i].WORLD_POSITION, linearNormal) * linearNormal);
        
        phongPosition += pointOnTangentPlane * barycentric[i];
    }
    
    const float alpha = 0.75f;
    
	DSOutput output;
    output.WORLD_POSITION = float4(lerp(linearPosition, phongPosition, alpha), 1.f);
    output.NORMAL = float4(linearNormal, 0.f);
    output.UV = linearUV;
    output.position = mul(output.WORLD_POSITION, vpMatrix);

	return output;
}
