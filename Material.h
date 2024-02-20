#pragma once
#include <DirectXMath.h>
#include "DXCore.h"


class Material
{
public:

private:
	DirectX::XMFLOAT3 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	bool finalized;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};

