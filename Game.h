#pragma once

#include "DXCore.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl/client.h> // Used for ComPtr -a smart pointer for COM objects

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);


private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();
	void CreateRootSigAndPipelineState();
	void CreateCamera();
	void LoadAndCreateAssets();
	void LoadMaterials();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	// entity list
	std::vector<GameEntity> entities;

	std::shared_ptr<Camera> camera;

	// bronze material
	std::shared_ptr<Material> bronzeMaterial;

	// wood material
	std::shared_ptr<Material> woodMaterial;

	//// **** lights ****

	//// directional
	//Light directionalLight1;
	//Light directionalLight2;
	//Light directionalLight3;

	//// point
	//Light pointLight1;
	//Light pointLight2;

	//// light vector
	//std::vector<Light> lights;
};

