float4 main() : SV_TARGET
{
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
    
    // Variables for various sin/cos values
    float sinT, cosT, sinP, cosP;
    
    // Loop around the hemisphere (360 degrees)
    for (float phi = 0.0f; phi < TWO_PI; phi += sampleStepPhi)
    {
        sincos(phi, sinP, cosP); // Grab the sin and cos of phi
        
        // Loop down the hemisphere (90 degrees)
        for (float theta = 0.0f; theta < PI_OVER_2; theta += sampleStepTheta)
        {
            sincos(theta, sinT, cosT); // Get the sin and cos of theta
            
            // Get an X/Y/Z direction from the polar coords, then change to world space
            float3 hemisphereDir = float3(sinT * cosP, sinT * sinP, cosT);
            hemisphereDir =
            hemisphereDir.x * xDir +
            hemisphereDir.y * yDir +
            hemisphereDir.z * zDir;
            
            // Sample in that direction
            totalColor += cosT * sinT * pow(abs(EnvironmentMap.Sample(SamplerOptions, hemisphereDir).rgb), 2.2f);
            sampleCount++;
        }
    }
    
    totalColor += cosT * sinT * pow(EnvironmentMapSample, 2.2f); // From shader
    // Cos() handles glancing angles (basically N dot L)
    
	return totalColor;
}
