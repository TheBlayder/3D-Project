// Back buffer UAV (from Renderer)
RWTexture2DArray<unorm float4> backBufferUAV : register(u0);

// G-buffer textures (from DeferredHandler)
Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> normalGBuffer : register(t1);
Texture2D<float4> ambientGBuffer : register(t2);
Texture2D<float4> diffuseGBuffer : register(t3);
Texture2D<float4> specularGBuffer : register(t4);

// Light data (from LightHandler)
cbuffer LightData : register(b0)
{
    int numSpotLights;
    int numDirectionalLights;
    float2 padding;
    float3 cameraPosition;
    float padding2;
};

struct Light
{
    float3 position;
    float intensity;
    float4 color;
    float3 direction;
    float padding;
};

// Structured buffers for lights (from LightHandler)
StructuredBuffer<Light> spotLights : register(t5);
StructuredBuffer<Light> directionalLights : register(t6);

const float ambientIntensity = 0.1f;

const int threadGroupSizeXY = 8;
[numthreads(threadGroupSizeXY, threadGroupSizeXY, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{  
    float3 position = positionGBuffer.Load(int3(DTid.xy, 0)).xyz;
    
    float4 ambientSample = ambientIntensity * ambientGBuffer.Load(int3(DTid.xy, 0));
    float4 diffuseSample = diffuseGBuffer.Load(int3(DTid.xy, 0));
    
    float3 color = ambientSample.xyz + diffuseSample.xyz;
    
    backBufferUAV[uint3(DTid.xy, 0)] = float4(color, 1.0f);
}