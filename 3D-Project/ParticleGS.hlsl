cbuffer CameraBuffer : register(b0)
{
    float4x4 vpMatrix;
    float3 cameraPosition;
    float padding;
};

struct GSInput
{
    float3 position : POSITION;
    float3 velocity : VELOCITY;
    float4 color : COLOR;
    float size : SIZE;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float4 NORMAL : NORMAL;
    float4 color : COLOR;
    float2 UV : UV;
};

[maxvertexcount(6)]
void main(
	point GSInput input[1] : POSITION,
	inout TriangleStream<GSOutput> output
)
{
    float3 frontVec = normalize(input[0].position - cameraPosition);
    float3 rightVec = normalize(cross(frontVec, float3(0.0, 1.0, 0.0)));
    float3 upVec = normalize(cross(frontVec, rightVec));
	
    rightVec = rightVec * 0.2f * input[0].size; // Scale width by size
    upVec = upVec * 0.5f * input[0].size; // Scale height by size
	
    GSOutput corners[4];
	
	// Top left
    corners[0].position = mul(float4(input[0].position - rightVec + upVec, 1.f), vpMatrix);
    corners[0].NORMAL = float4(frontVec, 0.f);
    corners[0].color = input[0].color;
    corners[0].UV = float2(0, 0);
	
	// Top right
    corners[1].position = mul(float4(input[0].position + rightVec + upVec, 1.f), vpMatrix);
    corners[1].NORMAL = float4(frontVec, 0.f);
    corners[1].color = input[0].color;
    corners[1].UV = float2(1, 0);
    
	// Bottom left
    corners[2].position = mul(float4(input[0].position - rightVec - upVec, 1.f), vpMatrix);
    corners[2].NORMAL = float4(frontVec, 0.f);
    corners[2].color = input[0].color;
    corners[2].UV = float2(0, 1);
	
	// Bottom right
    corners[3].position = mul(float4(input[0].position + rightVec - upVec, 1.f), vpMatrix);
    corners[3].NORMAL = float4(frontVec, 0.f);
    corners[3].color = input[0].color;
    corners[3].UV = float2(1, 1);
    
	
	// Triangle 1
    output.Append(corners[0]);
    output.Append(corners[1]);
    output.Append(corners[2]);
    output.RestartStrip();
	
	// Triangle 2
    output.Append(corners[2]);
    output.Append(corners[1]);
    output.Append(corners[3]);
    output.RestartStrip();
}