RWTexture2DArray<unorm float4> backBufferUAV : register(u0);

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> normalGBuffer : register(t1);
Texture2D<float4> ambientGBuffer : register(t2);
Texture2D<float4> diffuseGBuffer : register(t3);
Texture2D<float4> specularGBuffer : register(t4);


[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    // Bounds check to avoid out-of-range writes
    uint width, height;
    positionGBuffer.GetDimensions(width, height);
    if (DTid.x >= width || DTid.y >= height) return;
    
    // Read G-buffer position
    float3 position = positionGBuffer.Load(int3(DTid.xy, 0)).xyz;
    
    // If position is zero, assume no object (background). Otherwise combine ambient/diffuse/specular.
    float len2 = dot(position, position);
    float3 color = float3(0.f,0.f,0.f);
    
    if (len2 > 1e-6f)
    {
        float4 ambientSample = ambientGBuffer.Load(int3(DTid.xy,0));
        float4 diffuseSample = diffuseGBuffer.Load(int3(DTid.xy,0));
        
        color = diffuseSample.xyz;
    }
    
    backBufferUAV[uint3(DTid.xy, 0)] = float4(color, 1.0f);
}