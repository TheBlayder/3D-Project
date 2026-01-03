// Back buffer UAV (from Renderer)
RWTexture2DArray<unorm float4> backBufferUAV : register(u0);

// Shadow Sampler (from Renderer)
SamplerState shadowSampler : register(s0);

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

struct SpotLight
{
    float3 position;
    float intensity;
    float4 color;
    float3 direction;
    float innerConeInDeg;
    float outerConeInDeg;
    float range;
    float2 padding;
    float4x4 vpMatrix;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
    float4x4 vpMatrix;
};

// Structured buffers for lights (from LightHandler)
StructuredBuffer<SpotLight> spotLights : register(t5);
StructuredBuffer<DirectionalLight> directionalLights : register(t6);

// Shadow map depth textures (from LightHandler)
Texture2DArray<float> spotLightShadowMaps : register(t7);
Texture2DArray<float> directionalLightShadowMaps : register(t8);

static const int threadGroupSizeXY = 8;
[numthreads(threadGroupSizeXY, threadGroupSizeXY, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 diffuseComponent = 0.f;
    float4 specularComponent = 0.f;
    float specularExponent = specularGBuffer[DTid.xy].w;   
    
    float4 positionSample = float4(positionGBuffer[DTid.xy].xyz, 1.f);
    float4 normalSample = float4(normalize(normalGBuffer[DTid.xy].xyz), 0.f);
    float4 viewDirection_n = normalize(float4(cameraPosition, 0.f) - positionSample);
    
    const float depthBias = 0.001f;
    
    // Process spot lights
    for (int i = 0; i < nrOfSpotLights; ++i)
    {
        SpotLight light = spotLights[i];
        
        // Shadow calculation
        float4 lightClip = mul(positionSample, light.vpMatrix);
        if(lightClip.w <= 0.f)
            continue;
        float3 lightNDC = lightClip.xyz / lightClip.w;
        
        float3 lightUV = float3(lightNDC.x * 0.5f + 0.5f, lightNDC.y * -0.5f + 0.5f, i);
        float depth = spotLightShadowMaps.SampleLevel(shadowSampler, lightUV, 0).r;
        bool isInShadow = lightNDC.z > depth + depthBias;
        if (isInShadow)
            continue;
     
        // Check if pixel is within range of light.
        float4 lightToPixelVec = positionSample - float4(light.position, 0.f);
        if (length(lightToPixelVec) > light.range)
            continue;
        
        float4 lightToPixel_n = normalize(lightToPixelVec);
    
        // Check if pixel is facing away from the light
        if (dot(normalSample, -lightToPixel_n) <= 0.f)
            continue;
        
        float cosInnerCone = cos(radians(light.innerConeInDeg));
        float cosOuterCone = cos(radians(light.outerConeInDeg));
        
        // Calculate spotlight cone attenuation
        float theta = dot(normalize(float4(light.direction, 0.f)), lightToPixel_n);
        float epsilon = cosInnerCone - cosOuterCone;
        float spotAttenuation = clamp((theta - cosOuterCone) / epsilon, 0.f, 1.f);
        
        // Check if outside spotlight cone
        if (spotAttenuation <= 0.f)
            continue;
        
        // Diffuse calculation
        float irradience = max(dot(normalSample, -lightToPixel_n), 0.f); // Lambertian reflectance (amount of light hitting the surface)
        float distance = length(lightToPixelVec);
        float falloff = 1.0f / (distance * distance); // Inverse square falloff
        float combinedIntensity = falloff * light.intensity * spotAttenuation; // Shared variable to spare on computations
        
        diffuseComponent += combinedIntensity * irradience * light.color;
        
        // Specular calculation
        float4 halfVector_n = normalize(-lightToPixel_n + viewDirection_n);
        float angleHalfNormal = max(dot(normalSample, halfVector_n), 0.f);
        float specularIntensity = pow(max(angleHalfNormal, 0.f), specularExponent);
        
        specularComponent += specularIntensity * combinedIntensity * light.color;
    }
    
    // Process directional lights
    for (int i = 0; i < nrOfDirectionalLights; ++i)
    {
        DirectionalLight light = directionalLights[i];
        
        // Shadow calculation
        float4 lightClip = mul(positionSample, light.vpMatrix);
        float3 lightNDC = lightClip.xyz / lightClip.w;
        
        float3 lightUV = float3(lightNDC.x * 0.5f + 0.5f, lightNDC.y * -0.5f + 0.5f, i);
        float depth = directionalLightShadowMaps.SampleLevel(shadowSampler, lightUV, 0).r;
        bool isInShadow = lightNDC.z > depth + depthBias;
        if (isInShadow)
            continue;
        
        // Check if pixel is facing the light
        float4 direction_n = normalize(float4(light.direction, 0.f));
        float irradience = dot(normalSample, -direction_n);
        if(irradience <= 0.f)
            continue;
        
        // Diffuse
        float combinedIntensity = light.intensity * irradience;
        
        diffuseComponent += combinedIntensity * light.color;
        
        // Specular
        float4 halfVector_n = normalize(-direction_n + viewDirection_n);
        float angleHalfNormal = max(dot(normalSample, halfVector_n), 0.f);
        float specularIntensity = pow(max(angleHalfNormal, 0.f), specularExponent);
        
        specularComponent += combinedIntensity * specularIntensity * light.color;
    }
    
    float4 ambientFinal = ambientGBuffer[DTid.xy];
    float4 diffuseFinal = diffuseGBuffer[DTid.xy] * diffuseComponent;
    float4 specularFinal = float4(specularGBuffer[DTid.xy].xyz, 0.f) * specularComponent;
    
    backBufferUAV[float3(DTid.xy, 0.f)] = ambientFinal + diffuseFinal + specularFinal;
}