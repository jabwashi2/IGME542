#include "Material.h"

Material::Material(Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState, DirectX::XMFLOAT3 _colorTint, DirectX::XMFLOAT2 _uvScale, DirectX::XMFLOAT2 _uvOffset)
{
	// save passed in values
	this->pipelineState = _pipelineState;
	this->colorTint = _colorTint;
	this->uvScale = _uvScale;
	this->uvOffset = _uvOffset;

	// initialize other values
	this->finalized = false;
}

Material::~Material()
{
}

void Material::AddTexture(D3D12_CPU_DESCRIPTOR_HANDLE srv, int slot)
{
}

void Material::FinalizeMaterial()
{
}


// Getters

DirectX::XMFLOAT3 Material::GetColorTint()
{
	return colorTint;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return uvOffset;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> Material::GetPipeLineState()
{
	return pipelineState;
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetFinalGPUHandleForSRVs()
{
	return finalGPUHandleForSRVs;
}


// Setters

void Material::SetColorTint()
{
}

void Material::SetUVScale()
{
}

void Material::SetUVOffset()
{
}

void Material::SetPipelineState()
{
}
