#include "Lighting.hlsli"

#define MAX_SAMPLES         1024

cbuffer externalData : register(b0)
{
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
Texture2D EnvironmentMap : register(t0); // just one texture!
SamplerState BasicSampler : register(s0);


//Specular G
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float G1_Schlick(float Roughness, float NdotV)
{
    float k = Roughness * Roughness;
    k /= 2.0f; // Schlick-GGX version of k - Used in UE4

	// Staying the same
    return NdotV / (NdotV * (1.0f - k) + k);
}

// Specular G
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float G_Smith(float Roughness, float NdotV, float NdotL)
{
    return G1_Schlick(Roughness, NdotV) * G1_Schlick(Roughness, NdotL);
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
    float a = Roughness * Roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);
// Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float roughness = input.uv.y;
    float nDotV = input.uv.x;
    
    float3 V;
    V.x = sqrt(1.0f - nDotV * nDotV);
    V.y = 0;
    V.z = nDotV;
    
    float3 N = float3(0, 0, 1);

    float A = 0;
    float B = 0;
    
    for (uint i = 0; i < MAX_SAMPLES; i++)
    {
        float2 Xi = Hammersley2d(i, MAX_SAMPLES); // Evenly spaced 2D point from index
        float3 H = ImportanceSampleGGX(Xi, roughness, N); // Turn 2D point into 3D vector
        float3 L = 2 * dot(V, H) * H - V;
        float nDotL = saturate(L.z);
        float nDotH = saturate(H.z);
        float vDotH = saturate(dot(V, H));
        
        // Check N dot L result
        if (nDotL > 0)
        {
            float G = G_Smith(roughness, nDotV, nDotL);
            float G_Vis = G * vDotH / (nDotH * nDotV);
            float Fc = pow(1 - vDotH, 5);
            A += (1 - Fc) * G_Vis; // Fresnel scale (part of the output)
            B += Fc * G_Vis; // Fresnel bias (other part of the output)
        }
    }

    return float4(float2(A, B) / MAX_SAMPLES, 0.0f, 1.0f);
}