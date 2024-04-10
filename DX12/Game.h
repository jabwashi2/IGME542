#pragma once

#include "DXCore.h"
#include "Camera.h"
#include "GameEntity.h"
#include "Material.h"
#include "Lights.h"

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
	const int NUM_SPHERES = 10;

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadMeshes();
	void CreateGeometry();
	void CreateAssets();

	void LoadMaterials();

	void CreateCamera();
	void CreateLights();

	void CreateRootSigAndPipelineState();

	std::shared_ptr<Mesh> FindMesh(std::string meshName);
	std::shared_ptr<GameEntity> FindEntity(std::string entityName);

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

	// list of available meshes
	std::vector<std::shared_ptr<Mesh>> meshes;

	// entity list
	std::vector<std::shared_ptr<GameEntity>> entities;

	std::shared_ptr<Camera> camera;

	// bronze material
	std::shared_ptr<Material> bronzeMaterial;

	// wood material
	std::shared_ptr<Material> woodMaterial;

	// lights!
	int lightCount;
	std::vector<Light> lights;
};

