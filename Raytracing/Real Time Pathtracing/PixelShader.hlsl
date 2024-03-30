
#include "ShaderStructsInclude.hlsli"
#include "ShaderFunctionsInclude.hlsli"

#define TOTAL_LIGHTS 5
#define myTex2DSpace space1

// Alignment matters!!!
cbuffer ExternalData : register(b0)
{
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    int lightCount;
    Light lights[TOTAL_LIGHTS]; // array of lights
}

Texture2D Texture2DTable[] : register(t0, myTex2DSpace);

// smapler for textures!
SamplerState BasicSampler : register(s0);

// textures!
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
    // normalize normal and tangent
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);

    // uv scaling and offset
    input.uv = input.uv * uvScale + uvOffset;
    
    // normal mapping things
    float3 N = input.normal; // Must be normalized here or before
    float3 T = input.tangent; // Must be normalized here or before
    
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
     // unpack and sample normal
    float3 unpackedNormal = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    input.normal = normalize(mul(unpackedNormal, TBN)); 
    
    float3 surfaceColor = pow(AlbedoTexture.Sample(BasicSampler, input.uv).rgb, 2.2f); // surfaceColor only consists of albedo texture
    float roughness = RoughTexture.Sample(BasicSampler, input.uv).r;
    float metalness = MetalTexture.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);
    
    float3 finalColor = surfaceColor;
    
    float3 finalLight;
    for (int i = 0; i < TOTAL_LIGHTS; i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            finalLight = DirLight(lights[i], input.normal, surfaceColor, roughness, cameraPosition, input.worldPosition, specularColor, metalness);
        }
        if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            finalLight = PointLight(lights[i], input.normal, surfaceColor, roughness, cameraPosition, input.worldPosition, specularColor, metalness);
        }

        finalColor += finalLight;
    }
    

    return float4(pow(finalColor, 1.0f / 2.2f), 1);
}