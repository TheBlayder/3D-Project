RWTexture2DArray<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> normalGBuffer : register(t1);
Texture2D<float4> ambientGBuffer : register(t2);
Texture2D<float4> diffuseGBuffer : register(t3);
Texture2D<float4> specularGBuffer : register(t4);

const float ambientIntensity = 0.1f;

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Bounds check to avoid out-of-range writes
    uint width, height;
    positionGBuffer.GetDimensions(width, height);
    if (DTid.x >= width || DTid.y >= height) return;
    
    // Read G-buffer position
    float3 position = positionGBuffer.Load(int3(DTid.xy, 0)).xyz;
    
    float4 ambientSample = ambientIntensity * ambientGBuffer.Load(int3(DTid.xy, 0));
    float4 diffuseSample = diffuseGBuffer.Load(int3(DTid.xy, 0));
    
    float3 color = ambientSample.xyz + diffuseSample.xyz;
    
    backBufferUAV[uint3(DTid.xy, 0)] = float4(color, 1.0f);
}