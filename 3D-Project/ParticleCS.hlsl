
struct Particle
{
    float3 position;
    float3 velocity;
    float4 color;
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle gettingProcessed = Particles[DTid.x];
    
    
    
    
    
    
    
    
    
    Particles[DTid.x] = gettingProcessed;
}