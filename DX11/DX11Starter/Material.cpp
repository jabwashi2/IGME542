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

void Material::PrepareMaterial(std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera)
{
	// Turn on these shaders
	vertexShader->SetShader();
	pixelShader->SetShader();

	// Send data to the vertex shader
	vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix());
	vertexShader->SetMatrix4x4("worldInvTrans", transform->GetWorldInvTranspose());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());
	vertexShader->CopyAllBufferData();

	// Send data to the pixel shader
	pixelShader->SetFloat4("colorTint", colorTint);
	pixelShader->SetFloat("roughness", roughness);
	pixelShader->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
	pixelShader->SetFloat2("uvScale", XMFLOAT2(1, 1));
	pixelShader->SetFloat2("uvOffset", XMFLOAT2(1, 1));
	pixelShader->CopyAllBufferData();

	// Loop and set any other resources
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second.Get()); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second.Get()); }
}