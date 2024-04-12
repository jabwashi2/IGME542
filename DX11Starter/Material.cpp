#include "Material.h"

using namespace DirectX;


Material::Material(XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _pixelShader, std::shared_ptr<SimpleVertexShader> _vertexShader, float _roughness):
	vertexShader(_vertexShader),
	pixelShader(_pixelShader),
	colorTint(_colorTint),
	roughness(_roughness)
{
	// roughness will range from 0 to 1
}
Material::~Material()
{
	// nothing for now!
}

void Material::Setup()
{
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }
}

// **** getters ****

XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}
std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}
std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}
float Material::GetRoughness()
{
	return roughness;
}

// **** setters ****

void Material::SetColorTint(XMFLOAT4 newTint)
{
	colorTint = newTint;
}
void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader)
{
	pixelShader = newPixelShader;
}
void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader)
{
	vertexShader = newVertexShader;
}

// **** helpers ****

void Material::AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderName, srv });
}

void Material::AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sample)
{
	samplers.insert({ shaderName, sample });
}
