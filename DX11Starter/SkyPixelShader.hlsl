#include "ShaderStructsInclude.hlsli"
#include "ShaderFunctionsInclude.hlsli"

TextureCube SkyTexture : register(t0);

SamplerState SkySampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	float4 skySample = SkyTexture.Sample(SkySampler, input.sampleDir);

	return skySample;
}