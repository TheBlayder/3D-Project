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
    float3 velocity : VELOCITY;
	float4 color : COLOR;
    float size : SIZE;
};

[maxvertexcount(6)]
void main(
	point float3 input[1] : POSITION, 
	inout TriangleStream< GSOutput > output
)
{
    float3 frontVec = normalize(input[0] - cameraPosition);
    float3 rightVec = normalize(cross(frontVec, float3(0.0, 1.0, 0.0)));
	float3 upVec = normalize(cross(frontVec, rightVec));
	
    rightVec = rightVec * 0.2f; // Reduce width to closer resemble falling rain
    upVec = upVec * 0.5f;
	
    GSOutput corners[4];
	
	// Top left
    corners[0].position = mul(float4(input[0] - rightVec + upVec, 1.f), vpMatrix);
	
	// Bottom left
    corners[1].position = mul(float4(input[0] - rightVec - upVec, 1.f), vpMatrix);
	
	// Top right
    corners[2].position = mul(float4(input[0] + rightVec + upVec, 1.f), vpMatrix);
	
	// Bottom right
	corners[3].position = mul(float4(input[0] + rightVec - upVec, 1.f), vpMatrix);
    
	
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