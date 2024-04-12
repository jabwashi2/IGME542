cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float totalTime;
}

struct VertexToPixel
{

    float4 screenPosition : SV_POSITION;
    float2 uv : TEXTCOORD;

};

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color;

	// get the number out of the 0 to 1 range and round down
    float solidNumU = trunc(input.uv.x * 10);
    float solidNumV = trunc(input.uv.y * 10);
    
    if (solidNumV % 2)
    {
        color = float4(sin(input.uv.x), input.uv.x * totalTime, .6f, 1.0f) * frac(sin(dot(input.uv, float2(12.9898, 78.233))) * 43758.5453123);
    }
    else
    {
        color = float4(cos(input.uv.y), 0.0f, .6f, 1.0f) * frac(cos(dot(input.uv, float2(12.9898, 78.233))) * 43758.5453123);
    }
	
    return color;
}