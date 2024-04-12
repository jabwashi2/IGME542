#pragma once
#include <DirectXMath.h>
using namespace DirectX;

// defining light types
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT        2

struct Light 
{
	// this order is important!! types are interlaced to meet the 16-byte boundary
	int Type; // 0, 1, or 2
	XMFLOAT3 Direction; // used by directional and spot lights
	float Range; // used by point and spot lights for attenuation
	XMFLOAT3 Position; // used by point and spot lights (pos in space)
	float Intensity; // used by all
	XMFLOAT3 Color; // used by all
	float SpotFallOff; // used by spot lights to define cone size
	XMFLOAT3 Padding; // padding to hit the 16-byte boundary
};