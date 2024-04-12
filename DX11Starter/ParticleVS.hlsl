struct ParticleData
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
StructuredBuffer<ParticleData> ParticleDataSB : register(t0);

float4 main(uint id : SV_VertexID)
{
    VertexToPixel output;
    
    // particle info
    uint particleID = id / 4; // 4 vertices = one particle (ty Chris)
    
    ParticleData p = ParticleDataSB.Load(particleID);

    // Calculate the age and age "percentage" (0 to 1)
    float age = currentTime - p.EmitTime;
    float agePercent = age / lifetime;
    
    float3 pos = age * age / 2.0f + 2.0f * age + p.StartPosition;
    
    // Size interpolation
    float size = lerp(startSize, endSize, agePercent);
    
    // Offsets for the 4 corners of a quad - we'll only
	// use one for each vertex, but which one depends
	// on the cornerID above.
    float2 offsets[4];
    offsets[0] = float2(-1.0f, +1.0f); // TL
    offsets[1] = float2(+1.0f, +1.0f); // TR
    offsets[2] = float2(+1.0f, -1.0f); // BR
    offsets[3] = float2(-1.0f, -1.0f); // BL
    
    // Calculate output position
    matrix viewProj = mul(projection, view);
    output.position = mul(viewProj, float4(pos, 1.0f));
    
    // Finalize output
    output.colorTint = lerp(float4(0, 0, 0, 1), float4(0, 0, 0, 1), agePercent);
    
	return output;
}