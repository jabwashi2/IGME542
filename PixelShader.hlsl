
#include "ShaderStructsInclude.hlsli"

#define TOTAL_LIGHTS 5

// Alignment matters!!!
cbuffer ExternalData : register(b0)
{
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    int lightCount;
    Light lights[TOTAL_LIGHTS]; // array of lights
}

// smapler for textures!
SamplerState BasicSampler : register(s0);

Texture2D AlbedoTexture : register(t0);
Texture2D MetalTexture : register(t1);
Texture2D NormalTexture : register(t2);
Texture2D RoughTexture : register(t3);


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    
    float3 N = input.normal = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
	
    float3 surfaceColor = pow(AlbedoTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float roughness = RoughTexture.Sample(BasicSampler, input.uv).r;
    float metalness = MetalTexture.Sample(BasicSampler, input.uv).r;
    float3 unpackedNormal = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!

    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!

	
    return float4(surfaceColor, 1);
}