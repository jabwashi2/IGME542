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
SamplerComparisonState ShadowSampler : register(s1);

Texture2D SurfaceNormal : register(t0);
Texture2D SurfaceAlbedo : register(t1); 
Texture2D SurfaceRoughness : register(t2);
Texture2D SurfaceMetalness : register(t3);

Texture2D ShadowMap : register(t4);

float4 main(VertexToPixel_Normal input) : SV_TARGET
{
    // shadow map stuff!
    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
            ShadowSampler,
            shadowUV,
            distToLight).r;
    // **** end shadow
    
    float3 N = input.normal = normalize(input.normal); // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    // Adjust the variables below as necessary to work with your own code
    float3 surfaceColor = pow(SurfaceAlbedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    //float specScale = SurfaceSpecular.Sample(BasicSampler, input.uv).r;
    float roughness = SurfaceRoughness.Sample(BasicSampler, input.uv).r;
    float metalness = SurfaceMetalness.Sample(BasicSampler, input.uv).r;
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);
    
    
    // unpack and sample normal
    float3 unpackedNormal = SurfaceNormal.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!
    
    float3 finalColor = ambientColor * surfaceColor;
    
    // Assumes that input.normal is the normal later in the shader
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!
    
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
        
        if (i == 0 && lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            finalLight *= shadowAmount;
        }
        finalColor += finalLight;
    }
    
    return float4(pow(finalColor, 1.0f / 2.2f), 1);
}