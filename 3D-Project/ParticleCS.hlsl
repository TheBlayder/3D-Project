
struct Particle
{
    float3 position;
    float size;
    float3 velocity;
    float padding;
    float4 color;
};

cbuffer TimeBuffer : register(b1)
{
    float deltaTime;
    float3 padding; // Vet inte vad mer jag behöver här än
};

RWStructuredBuffer<Particle> Particles : register(u1);

// Hash function for pseudo-random number generation
float hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return float(seed) * (1.0 / 4294967296.0);
}

// Returns a random float2 value clamped between -10 and 10
float2 getRandomFloat2(uint seed)
{
    float x = hash(seed) * 20.0f - 10.0f;
    float y = hash(seed + 1) * 20.0f - 10.0f;
    return float2(x, y);
}

const float gravity = -9.82f;
const float floorThreshold = -1.0f;

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle gettingProcessed = Particles[DTid.x];
    
     // Reset particle if below threshhold
    if (gettingProcessed.position.y < floorThreshold)
    {
        float2 randomValue = getRandomFloat2(DTid.x);
        gettingProcessed.position.y = 40.0f + randomValue.x;
        gettingProcessed.velocity.y = 40.0f;
        
        randomValue = getRandomFloat2(DTid.x);
        gettingProcessed.position.x = randomValue.x;
        gettingProcessed.position.z = randomValue.y + 10.f;
    }
    
    // Update Particle Position (d = v0*t + (a*t^2)/2)
    float distanceMoved = gettingProcessed.velocity.y * deltaTime + gravity * deltaTime * deltaTime / 2;
    gettingProcessed.position.y += distanceMoved;
    
    // Update Particle Velocity (v = v0 + a*t)
    gettingProcessed.velocity.y = gettingProcessed.velocity.y + gravity * deltaTime;
    
    Particles[DTid.x] = gettingProcessed;
}