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
    int nrOfSpotLights;
    int nrOfDirectionalLights;
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

const int threadGroupSizeXY = 8;
[numthreads(threadGroupSizeXY, threadGroupSizeXY, 1)]
//void main( uint3 DTid : SV_DispatchThreadID )
//{  
//    float3 position = positionGBuffer.Load(int3(DTid.xy, 0)).xyz;
    
//    float4 ambientSample = ambientGBuffer.Load(int3(DTid.xy, 0));
//    float4 diffuseSample = diffuseGBuffer.Load(int3(DTid.xy, 0));
    
//    float3 color = ambientSample.xyz + diffuseSample.xyz;
    
//    backBufferUAV[uint3(DTid.xy, 0)] = float4(color, 1.0f);
//}

void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 diffuseComponent = 0;
    float4 specularComponent = 0;
    float specularExponent = specularGBuffer[DTid.xy].w;   
    
    float4 positionSample = float4(positionGBuffer[DTid.xy].xyz, 0);
    float4 normalSample = float4(normalize(normalGBuffer[DTid.xy].xyz), 0);
    float4 viewDirection = normalize(float4(cameraPosition, 0) - positionSample);
    
    // Process spot lights
    for (int i = 0; i < nrOfSpotLights; ++i)
    {
        Light light = spotLights[i];
        
        float4 lightDirection = normalize(float4(light.position, 0) - positionSample);
        
        // Check if pixel is facing the light
        if (dot(normalSample, lightDirection) <= 0)
            continue;
        
        // Diffuse calculation
        float irradience = max(dot(normalSample, lightDirection), 0.0f); // Lambertian reflectance (amount of light hitting the surface)
        float falloff = 1.0f / dot(-lightDirection, -lightDirection); // Inverse square falloff
        float combinedIntensity = falloff * light.intensity * light.color; // Shared variable to spare on computations
        
        diffuseComponent += combinedIntensity * irradience;
        
        // Specular calculation
        float4 halfVector = normalize(-lightDirection + viewDirection);
        float angleHalfNormal = max(dot(normalSample, halfVector), 0.0f);
        float specularIntensity = (irradience > 0) ? pow(max(angleHalfNormal, 0), specularExponent) : 0;
        
        specularComponent += specularIntensity * combinedIntensity;
    }
    
    // Process directional lights
    for (int i = 0; i < nrOfDirectionalLights; ++i)
    {
        Light light = directionalLights[i];
        
    }
    
    float4 ambientFinal = ambientGBuffer[DTid.xy];
    float4 diffuseFinal = diffuseGBuffer[DTid.xy] * diffuseComponent;
    float4 specularFinal = float4(specularGBuffer[DTid.xy].xyz, 0) * specularComponent;
    
    backBufferUAV[uint3(DTid.xy, 0)] = ambientFinal + diffuseFinal + specularFinal;
}