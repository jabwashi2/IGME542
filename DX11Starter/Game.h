#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "PathHelpers.h"
#include "Lights.h"
#include "Sky.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// **** Overridden setup and game loop methods, which will be called automatically ****
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	DirectX::XMFLOAT4 editColor;

	// **** cameras ****
	std::shared_ptr<Camera> activeCam;
	std::shared_ptr<Camera> cam1;
	std::shared_ptr<Camera> cam2;
	std::shared_ptr<Camera> cam3;
	std::vector<std::shared_ptr<Camera>> cameras;

	// **** materials ****
	std::shared_ptr<Material> material1;
	std::shared_ptr<Material> material2;
	DirectX::XMFLOAT3 ambientColor;

	// **** skybox ****
	std::shared_ptr<Sky> skybox;

private:

	// **** Initialization helper methods - feel free to customize, combine, remove, etc. ****

	void LoadShaders();
	void LoadMaterials();
	void CreateShadowMap();
	void CreateSky();
	void CreateCameras();
	void CreateGeometry();
	void PostProcessingSetup();

	// ImGUI Helpers
	void ImGuiHelper(float dt, std::vector<GameEntity> _entities, std::vector< std::shared_ptr<Camera>> _cameras);
	void ImGuiSetup(float dt);

	// **** Buffers to hold actual geometry data ****
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// **** Shaders and shader-related constructs ****
	
	// texture stuff
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler; // for now, just one >:)

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	// my custom shader!
	std::shared_ptr<SimplePixelShader> customPixelShader;

	// normal map shaders
	std::shared_ptr<SimpleVertexShader> normalVertexShader;
	std::shared_ptr<SimplePixelShader> normalPixelShader;

	// skybox shaders
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
	std::shared_ptr<SimplePixelShader> skyPixelShader;

	// shadow rasterizer state
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;

	// shadow sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;

	// shadow shader
	std::shared_ptr<SimpleVertexShader> shadowVertexShader;

	// entity list
	std::vector<GameEntity> entities;

	// **** lights ****

	// directional
	Light directionalLight1;
	Light directionalLight2;
	Light directionalLight3;

	// point
	Light pointLight1;
	Light pointLight2;

	// light vector
	std::vector<Light> lights;

	// shadow mapping
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	// **** Post-processing things ****
	// note: multiple post processing render targets can share a vertex shader,
	//	but each needs its own render target view (RTV), shader resource view (SRV), and pixel shader

	// Resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVertexShader;

	// Resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPixelShader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling

	int blurRadius;

};

