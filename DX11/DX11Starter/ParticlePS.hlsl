#include "ShaderStructsInclude.hlsli"

Texture2D ParticleTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Particle input) : SV_TARGET
{
    return ParticleTexture.Sample(BasicSampler, input.uv) * input.colorTint;
}