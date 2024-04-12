struct Particle
{
    float EmitTime;
    float3 StartPosition;
};

cbuffer externalData : register(b0)
{
    matrix view;
    matrix projection;

    float startSize;
    float endSize;
    float lifetime;
    float currentTime;

};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 colorTint : COLOR;
};

// Buffer of particle data
StructuredBuffer<Particle> ParticleData : register(t0);

float4 main( uint id : SV_VertexID)
{
    VertexToPixel output;
    
    // particle info
    uint particleID = id / 4; // 4 vertices = one particle (ty Chris)
    
    // Grab one particle and calculate its age
    Particle p = ParticleData.Load(particleID);
    float age = currentTime - p.EmitTime; // currentTime is from C++
    
	return output;
}