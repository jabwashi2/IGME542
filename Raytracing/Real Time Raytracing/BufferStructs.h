#pragma once

#include "Lights.h"
#include <DirectXMath.h>

struct VertexShaderExternalData
{
	// alignment is good!
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
};

struct PixelShaderExternalData
{
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT3 cameraPosition;
	int lightCount;
	Light lights[TOTAL_LIGHTS];
};

// Overall scene data for raytracing
struct RaytracingSceneData
{
	DirectX::XMFLOAT4X4 inverseViewProjection;
	DirectX::XMFLOAT3 cameraPosition;
};

// Ensure this matches Raytracing shader define!
#define MAX_INSTANCES_PER_BLAS 100
struct RaytracingEntityData
{
	DirectX::XMFLOAT4 color[MAX_INSTANCES_PER_BLAS];
};
