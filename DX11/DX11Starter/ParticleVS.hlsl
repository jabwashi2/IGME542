#include "ShaderStructsInclude.hlsli"

struct ParticleData
{
    float EmitTime;
    float3 StartPosition;
};

cbuffer externalData : register(b0)
{
    matrix view;
    matrix projection;
    float currentTime;
    
    float startSize;
    float endSize;
    float lifetime;
};

// Buffer of particle data
StructuredBuffer<ParticleData> ParticleDataSB : register(t0);

VertexToPixel_Particle main(uint id : SV_VertexID)
{
    VertexToPixel_Particle output;
    
    // particle info
    uint particleID = id / 4; // Every group of 4 verts are ONE particle! (int division)
    uint cornerID = id % 4; // 0,1,2,3 = which corner of the particle’s "quad"
    ParticleData p = ParticleDataSB.Load(particleID); // Each vertex gets associated particle!
    
    float age = currentTime - p.EmitTime;
    
    float3 pos = p.StartPosition + age * 2;
    
    // billboarding
    float2 offsets[4];
    offsets[0] = float2(-1.0f, +1.0f); // Top Left
    offsets[1] = float2(+1.0f, +1.0f); // Top Right
    offsets[2] = float2(+1.0f, -1.0f); // Bottom Right
    offsets[3] = float2(-1.0f, -1.0f); // Bottom Left
    
    // Billboarding!
    // Offset the position based on the camera's right and up vectors
    pos += float3(view._11, view._12, view._13) * offsets[cornerID].x; // RIGHT
    pos += float3(view._21, view._22, view._23) * offsets[cornerID].y; // UP
    
    // Finally, calculate output position here using View and Projection matrices
    matrix viewProj = mul(projection, view);
    output.position = mul(viewProj, float4(pos, 1.0f));
    
    float2 uvs[4];
    uvs[0] = float2(0, 0); // TL
    uvs[1] = float2(1, 0); // TR
    uvs[2] = float2(1, 1); // BR
    uvs[3] = float2(0, 1); // BL
    output.uv = uvs[cornerID];

      
    return output;
}