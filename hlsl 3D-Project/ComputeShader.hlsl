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
    
    // IMPORTANT: position should be treated as a point (w = 1) when projecting to light space
    float4 positionSample = float4(positionGBuffer[DTid.xy].xyz, 1.0f);
    float4 normalSample = float4(normalize(normalGBuffer[DTid.xy].xyz), 0.f);

    // compute view direction as float3 (positionSample.xyz is a point)
    float3 viewDir = normalize(cameraPosition - positionSample.xyz);
    float4 viewDirection_n = float4(viewDir, 0.0f);

    // Process spot lights
    for (int i = 0; i < nrOfSpotLights; ++i)
    {
        SpotLight light = spotLights[i];
        
        // Shadow calculation
        float4 lightClip = mul(light.vpMatrix, positionSample);
        // Handle w <= 0 to avoid invalid projection
        if (lightClip.w == 0.f) continue;

        float3 lightNDC = lightClip.xyz / lightClip.w;

        // Early out if outside light NDC cube, treat as lit (or skip shadow test)
        if (lightNDC.x < -1.0f || lightNDC.x > 1.0f ||
            lightNDC.y < -1.0f || lightNDC.y > 1.0f ||
            lightNDC.z < 0.0f  || lightNDC.z > 1.0f)
        {
            // Not inside shadow map frustum -> treat as lit by this light
        }
        else
        {
            float3 lightUV = float3(lightNDC.x * 0.5f + 0.5f, lightNDC.y * -0.5f + 0.5f, i);
            float depth = spotLightShadowMaps.SampleLevel(shadowSampler, lightUV, 0).r;
            bool isInShadow = lightNDC.z > depth + 0.005f;
            if (isInShadow)
                continue;
        }

        // Check if pixel is within range of light.
        float3 lightToPixelVec3 = positionSample.xyz - light.position;
        if (length(lightToPixelVec3) > light.range)
            continue;
        
        float3 lightToPixel_n = normalize(lightToPixelVec3);
    
        // Check if pixel is facing away from the light
        if (dot(normalSample.xyz, -lightToPixel_n) <= 0.f)
            continue;
        
        float cosInnerCone = cos(radians(light.innerConeInDeg));
        float cosOuterCone = cos(radians(light.outerConeInDeg));
        
        // Calculate spotlight cone attenuation
        float theta = dot(normalize(light.direction), lightToPixel_n);
        float epsilon = cosInnerCone - cosOuterCone;
        float spotAttenuation = clamp((theta - cosOuterCone) / epsilon, 0.f, 1.f);
        
        // Check if outside spotlight cone
        if (spotAttenuation <= 0.f)
            continue;
        
        // Diffuse calculation
        float irradience = max(dot(normalSample.xyz, -lightToPixel_n), 0.f); // Lambertian
        float distance = length(lightToPixelVec3);
        float falloff = 1.0f / (distance * distance); // inverse-square
        float combinedIntensity = falloff * light.intensity * spotAttenuation;
        
        diffuseComponent += combinedIntensity * irradience * light.color;
        
        // Specular calculation
        float4 halfVector_n = normalize(float4(-lightToPixel_n + viewDir, 0.f));
        float angleHalfNormal = max(dot(normalSample.xyz, halfVector_n.xyz), 0.f);
        float specularIntensity = pow(max(angleHalfNormal, 0.f), specularExponent);
        
        specularComponent += specularIntensity * combinedIntensity * light.color;
    }
    
    // Process directional lights
    for (int i = 0; i < nrOfDirectionalLights; ++i)
    {
        DirectionalLight light = directionalLights[i];
        
        // Shadow calculation
        float4 lightClip = mul(light.vpMatrix, positionSample);
        if (lightClip.w == 0.f) continue;
        float3 lightNDC = lightClip.xyz / lightClip.w;

        if (lightNDC.x < -1.0f || lightNDC.x > 1.0f ||
            lightNDC.y < -1.0f || lightNDC.y > 1.0f ||
            lightNDC.z < 0.0f  || lightNDC.z > 1.0f)
        {
            // outside sun frustum -> treat as lit
        }
        else
        {
            float3 lightUV = float3(lightNDC.x * 0.5f + 0.5f, lightNDC.y * -0.5f + 0.5f, i);
            float depth = directionalLightShadowMaps.SampleLevel(shadowSampler, lightUV, 0).r;
            bool isInShadow = lightNDC.z > depth + 0.005f;
            if(isInShadow)
                continue;
        }
        
        // Check if pixel is facing the light
        float3 direction_n = normalize(light.direction);
        float irradience = dot(normalSample.xyz, -direction_n);
        if(irradience <= 0.f)
            continue;
        
        // Diffuse
        float combinedIntensity = light.intensity * irradience;
        
        diffuseComponent += combinedIntensity * light.color;
        
        // Specular
        float4 halfVector_n = normalize(float4(-direction_n + viewDir, 0.f));
        float angleHalfNormal = max(dot(normalSample.xyz, halfVector_n.xyz), 0.f);
        float specularIntensity = pow(max(angleHalfNormal, 0.f), specularExponent);
        
        specularComponent += combinedIntensity * specularIntensity * light.color;
    }
    
    float4 ambientFinal = ambientGBuffer[DTid.xy];
    float4 diffuseFinal = diffuseGBuffer[DTid.xy] * diffuseComponent;
    float4 specularFinal = float4(specularGBuffer[DTid.xy].xyz, 0.f) * specularComponent;
    
    backBufferUAV[float3(DTid.xy, 0.f)] = ambientFinal + diffuseFinal + specularFinal;
}