float4 main() : SV_TARGET
{
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
            float G = G_Smith(roughnessValue, nDotV, nDotL);
            float G_Vis = G * vDotH / (nDotH * nDotV);
            float Fc = pow(1 - vDotH, 5);
            A += (1 - Fc) * G_Vis; // Fresnel scale (part of the output)
            B += Fc * G_Vis; // Fresnel bias (other part of the output)
        }
    }

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}