#ifndef __GGP_SHADER_INCLUDES_STRUCTS__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES_STRUCTS__

// defining light types
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT        2
#define MAX_SPECULAR_EXPONENT 256.0f

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXTCOORD;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR; // RGBA color
    float3 normal : NORMAL; 
    float3 worldPosition : POSITION;
    float2 uv : TEXTCOORD;
    float4 shadowMapPos : SHADOW_POSITION;
};

struct VertexToPixel_Normal // currently being used
{
    float4 screenPosition : SV_POSITION;
    float4 color : COLOR; // RGBA color
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
    float2 uv : TEXTCOORD;
    float4 shadowMapPos : SHADOW_POSITION;
};

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

struct Light
{
	// this order is important!! types are interlaced to meet the 16-byte boundary
    int Type; // 0, 1, or 2
    float3 Direction; // used by directional and spot lights
    float Range; // used by point and spot lights for attenuation
    float3 Position; // used by point and spot lights (pos in space)
    float Intensity; // used by all
    float3 Color; // used by all
    float SpotFallOff; // used by spot lights to define cone size
    float3 Padding; // padding to hit the 16-byte boundary
};

#endif