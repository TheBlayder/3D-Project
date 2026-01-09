
cbuffer Camera : register(b0)
{
    float4x4 vpMatrix;
};

struct HSOutput
{
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

struct DSOutput
{
	float4 position : SV_POSITION;
    float4 worldPosition : WORLD_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DSOutput main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 barycentric : SV_DomainLocation,
	const OutputPatch<HSOutput, NUM_CONTROL_POINTS> patch)
{
    
    float3 linearPosition = patch[0].worldPosition * barycentric.x + patch[1].worldPosition * barycentric.y + patch[2].worldPosition * barycentric.z;
    float3 linearNormal = normalize(patch[0].normal * barycentric.x + patch[1].normal * barycentric.y + patch[2].normal * barycentric.z);
    float2 linearUV = patch[0].uv * barycentric.x + patch[1].uv * barycentric.y + patch[2].uv * barycentric.z;
    
    float3 phongPosition = 0.f;
    
    for (int i = 0; i < NUM_CONTROL_POINTS; ++i)
    {
        float3 pointOnTangentPlane = linearPosition - (dot(linearPosition - patch[i].worldPosition, linearNormal) * linearNormal);
        
        phongPosition += pointOnTangentPlane * barycentric[i];
    }
    
    const float alpha = 0.75f;
    
	DSOutput output;
    output.worldPosition = float4(lerp(linearPosition, phongPosition, alpha), 1.f);
    output.normal = float4(linearNormal, 0.f);
    output.uv = linearUV;
    output.position = mul(output.worldPosition, vpMatrix);

	return output;
}
