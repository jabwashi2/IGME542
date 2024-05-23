#include "Lighting.hlsli"

#define MAX_SAMPLES         1024

cbuffer externalData : register(b0)
{
    int roughness;
    int faceIndex;
};

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPos : POSITION; // The world position of this PIXEL
};

// Textures and samplers
TextureCube EnvironmentMap : register(t0);
SamplerState BasicSampler : register(s0);

// From the book Hacker’s Delight – Some ridiculous bitwise math
float radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
    float a = roughness * roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    float3 UpVector = abs(N.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);
    
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}
float2 Hammersley2d(uint i, uint N)
{
    return float2(float(i) / float(N), radicalInverse_VdC(i));
}


float4 main(VertexToPixel input) : SV_TARGET
{
    float3 zDir;
	// Get a -1 to 1 range on x/y
    float2 o = input.uv * 2 - 1;
    // Figure out the z ("normal" of this pixel)
    switch (faceIndex)
    {
        default:
        case 0:
            zDir = float3(+1, -o.y, -o.x);
            break;
        case 1:
            zDir = float3(-1, -o.y, +o.x);
            break;
        case 2:
            zDir = float3(+o.x, +1, +o.y);
            break;
        case 3:
            zDir = float3(+o.x, -1, -o.y);
            break;
        case 4:
            zDir = float3(+o.x, -o.y, +1);
            break;
        case 5:
            zDir = float3(-o.x, -o.y, -1);
            break;
    }
    zDir = normalize(zDir);
    float3 finalColor = float3(0, 0, 0);
    float totalWeight = 0;
    
    float3 N = zDir;
    float3 V = zDir;
    
	// Run the calculation MANY times
    for (uint i = 0; i < MAX_SAMPLES; i++)
    {
        // Grab this sample
        float2 Xi = Hammersley2d(i, MAX_SAMPLES); // Evenly spaced 2D point from index
        float3 H = ImportanceSampleGGX(Xi, roughness, N); // Turn 2D point into 3D vector
        float3 L = 2 * dot(V, H) * H - V;
        
        // Check N dot L result
        float nDotL = saturate(dot(N, L));
        if (nDotL > 0)
        {
            float3 thisColor = (EnvironmentMap.SampleLevel(BasicSampler, L, 0).rgb);
            thisColor = pow(abs(thisColor), 2.2f); // Using abs() to stop a compiler warning
            finalColor += thisColor * nDotL;
            totalWeight += nDotL;
        }
    }
	
    return float4(pow(abs(finalColor / totalWeight), 1.0f / 2.2f), 1);
}