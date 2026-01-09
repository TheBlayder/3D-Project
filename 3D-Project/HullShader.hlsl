
cbuffer DistanceToObjectCenter : register(b0)
{
    float distance;
};

struct VSOutput
{
    float3 worldPos : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3] : SV_TessFactor;
	float InsideTessFactor : SV_InsideTessFactor;
};

struct HSOutput
{
    float3 worldPos : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VSOutput, NUM_CONTROL_POINTS> ip)
{
	HS_CONSTANT_DATA_OUTPUT output;
	
    float maxDistance = 60.f;
    float baseTessFactor = 32.f;
    float minTessFactor = 1.f;
	
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

	output.worldPos = ip[i].worldPos;
	output.normal = ip[i].normal;
    output.uv = ip[i].uv;

	return output;
}
