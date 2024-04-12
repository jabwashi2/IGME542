#pragma once

#include "SimpleShader.h"
#include <iostream>
#include <DirectXMath.h>
#include <unordered_map>

class Material
{
public:
	Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _pixelShader, std::shared_ptr<SimpleVertexShader> _vertexShader, float _roughness);
	~Material();

	void Setup();

	// **** getters ****
	float GetRoughness();
	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();

	// **** setters ****
	void SetColorTint(DirectX::XMFLOAT4 newTint);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader);

	// **** SRVs ****
	void AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sample);

	// **** for the textures ****
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	float roughness;
};

