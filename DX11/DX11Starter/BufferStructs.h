#pragma once

#include <DirectXMath.h>

struct VertexShaderExternalData
{
	// alignment is good!
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
};
