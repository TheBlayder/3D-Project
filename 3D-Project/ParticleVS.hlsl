struct Particle
{
    float3 position;
    float3 velocity;
    float4 color;
    float size;
};

struct VSOutput
{
    float3 position : POSITION;
    float3 velocity : VELOCITY;
    float4 color : COLOR;
    float size : SIZE;
};

StructuredBuffer<Particle> Particles : register(t0);

VSOutput main( uint vertexID : SV_VertexID ) : POSITION
{
    Particle p = Particles[vertexID];
    
    VSOutput output;
    output.position = p.position;
    output.velocity = p.velocity;
    output.color = p.color;
    output.size = p.size;
    
    return output;
}