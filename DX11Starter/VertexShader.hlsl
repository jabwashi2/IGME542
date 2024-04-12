#include "ShaderStructsInclude.hlsli"

cbuffer ExternalData : register(b0)
{
	// matrices
	float4x4 world;
	float4x4 worldInvTranspose;
	float4x4 view;
	float4x4 projection;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz; // local pos x world matrix (only first 3 components of the vector)
	output.normal = mul((float3x3)worldInvTranspose, input.normal); // inverse transpose of world matrix cast to 3x3 x input normal
    output.uv = input.uv;
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}