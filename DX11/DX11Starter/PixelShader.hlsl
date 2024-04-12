#include "ShaderStructsInclude.hlsli"
#include "ShaderFunctionsInclude.hlsli"

#define TOTAL_LIGHTS 5

cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    
    // lights array!
    Light lights[TOTAL_LIGHTS];
}

// smapler for textures!
SamplerState BasicSampler : register(s0);

// you may end up with multiple of these; will be used for this is angle draw
Texture2D SurfaceTexture : register(t0);
Texture2D SurfaceSpecular : register(t1);

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
    // normalize lights
    input.normal = normalize(input.normal);
    
    // Adjust the variables below as necessary to work with your own code
    float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float specScale = SurfaceSpecular.Sample(BasicSampler, input.uv).r;
    
    // after making 3 lights, add together with ambientColor to make finalColor; return final color
    float3 finalColor = ambientColor * surfaceColor;
    
    float3 finalLight;
    for (int i = 0; i < TOTAL_LIGHTS; i++)
    {
        
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            finalLight = DirLight(lights[i], input.normal, surfaceColor, roughness, cameraPosition, input.worldPosition, specScale, 0.0f);
        }
        if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            finalLight = PointLight(lights[i], input.normal, surfaceColor, roughness, cameraPosition, input.worldPosition, specScale, 0.0f);
        }
        finalColor += finalLight;
    }
    
    return float4(pow(finalColor, 1.0f/2.2f), 1);
}