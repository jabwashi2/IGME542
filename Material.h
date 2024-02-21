#pragma once
#include <DirectXMath.h>
#include "DXCore.h"


class Material
{
public:
	Material(Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState, DirectX::XMFLOAT3 _colorTint, DirectX::XMFLOAT2 _uvScale = DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT2 _uvOffset = DirectX::XMFLOAT2(0,0));
	~Material();

	void AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot);
	void FinalizeMaterial();

	// getters
	DirectX::XMFLOAT3 GetColorTint();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipeLineState();
	D3D12_GPU_DESCRIPTOR_HANDLE GetFinalGPUHandleForSRVs();

	// setters
	void SetColorTint(DirectX::XMFLOAT3 newColor);
	void SetUVScale(DirectX::XMFLOAT2 newScale);
	void SetUVOffset(DirectX::XMFLOAT2 newOffset);
	void SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> newPipeline);

private:
	bool finalized;

	DirectX::XMFLOAT3 colorTint;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSRVsBySlot[4];
	D3D12_GPU_DESCRIPTOR_HANDLE finalGPUHandleForSRVs;
};

