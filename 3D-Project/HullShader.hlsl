
cbuffer DistanceToObjectCenter : register(b0)
{
    float distance;
    float3 padding;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 WORLD_POSITION : WORLD_POSITION;
    float4 NORMAL : NORMAL;
    float2 UV : UV;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3] : SV_TessFactor;
	float InsideTessFactor : SV_InsideTessFactor;
};

struct HSOutput
{
    float3 WORLD_POSITION : WORLD_POSITION;
    float3 NORMAL : NORMAL;
    float2 UV : UV;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VSOutput, NUM_CONTROL_POINTS> ip)
{
	HS_CONSTANT_DATA_OUTPUT output;
	
    float maxDistance = 40.f;
    float baseTessFactor = 16.f;
    float minTessFactor = 0.1f;
	
    float tessFactor = lerp(baseTessFactor, minTessFactor, saturate(distance / maxDistance));

	output.EdgeTessFactor[0] = 
		output.EdgeTessFactor[1] = 
		output.EdgeTessFactor[2] = 
		output.InsideTessFactor = tessFactor;

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HSOutput main(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID)
{
    HSOutput output;

    output.WORLD_POSITION.xyz = ip[i].WORLD_POSITION.xyz;
    output.NORMAL.xyz = ip[i].NORMAL.xyz;
    output.UV = ip[i].UV;

	return output;
}
