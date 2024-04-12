#pragma once

#include "SimpleShader.h"
#include "Mesh.h"
#include "Camera.h"
#include "WICTextureLoader.h"

#include <iostream>
#include <DirectXMath.h>

class Sky
{
public:
	// will pass in texture filepaths from Game.cpp; will use cube mesh loaded in Game.cpp
	Sky(std::shared_ptr<Mesh> mesh_ptr, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<SimpleVertexShader> vs, const wchar_t* text1, const wchar_t* text2, const wchar_t* text3, const wchar_t* text4, const wchar_t* text5, const wchar_t* text6);
	~Sky();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSkySRV();
	void Draw(std::shared_ptr<Camera> camPtr);

private:
	std::shared_ptr<Mesh> skyMesh;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> skySampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterizer;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// --------------------------------------------------------
	// Author: Chris Cascioli
	// Purpose: Creates a cube map on the GPU from 6 individual textures
	// 
	// - You are allowed to directly copy/paste this into your code base
	//   for assignments, given that you clearly cite that this is not
	//   code of your own design.
	//
	// - Note: This code assumes you’re putting the function in Sky.cpp, 
	//   you’ve included WICTextureLoader.h and you have an ID3D11Device 
	//   ComPtr called “device”.  Make any adjustments necessary for
	//   your own implementation.
	// --------------------------------------------------------
	// --- HEADER ---
	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};