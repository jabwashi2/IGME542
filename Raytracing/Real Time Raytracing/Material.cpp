#include "Material.h"
#include "DX12Helper.h"

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
	// adding the srv to the array based on the slot (index)
	textureSRVsBySlot[slot] = srv;
}

void Material::FinalizeMaterial()
{
	// make sure material has not been finalized
	if (!finalized) {
		int loopnum = 0;
		// copy one srv at a time in a loop
		for (auto& s : textureSRVsBySlot) {
			loopnum++;
			// call dx12 function to copy srvs
			D3D12_GPU_DESCRIPTOR_HANDLE currentHandle = DX12Helper::GetInstance().CopySRVsToDescriptorHeapAndGetGPUDescriptorHandle(s, 1);

			if (loopnum == 1) {
				finalGPUHandleForSRVs = currentHandle;
			}
		}
		finalized = true;
	}
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

void Material::SetColorTint(DirectX::XMFLOAT3 newColor)
{
	this->colorTint = newColor;
}

void Material::SetUVScale(DirectX::XMFLOAT2 newScale)
{
	this->uvScale = newScale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 newOffset)
{
	this->uvOffset = newOffset;
}

void Material::SetPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> newPipeline)
{
	this->pipelineState = newPipeline;
}
