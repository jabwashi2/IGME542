
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
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;
	
    matrix wvp = mul(projection, mul(view, world));
	
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.normal = normalize(mul((float3x3)worldInvTranspose, input.normal));
    output.tangent = normalize(mul((float3x3)world, input.tangent));
	
    output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;
	
    output.uv = input.uv;

	return output;
}