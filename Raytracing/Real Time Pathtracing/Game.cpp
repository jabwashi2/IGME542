#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "DX12Helper.h"
#include "BufferStructs.h"
#include "Camera.h"
#include "Lights.h"
#include "RaytracingHelper.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	ibView = {};
	vbView = {};

	entities = {};

	lights = {};

	lightCount = 5;
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// We need to wait here until the GPU
	// is actually done with its work
	DX12Helper::GetInstance().WaitForGPU();

	delete& RaytracingHelper::GetInstance();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Attempt to initialize DXR
	RaytracingHelper::GetInstance().Initialize(
		windowWidth,
		windowHeight,
		device,
		commandQueue,
		commandList,
		FixPath(L"Raytracing.cso"));

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.

	CreateRootSigAndPipelineState();

	LoadMeshes();

	LoadMaterials();

	CreateGeometry();

	CreateCamera();
}

#pragma region helper functions

void Game::LoadMeshes()
{
	// cube
	meshes.push_back(std::make_shared<Mesh>("cube", FixPath("../../Assets/cube.obj").c_str(), device, commandList));

	// cylinder
	meshes.push_back(std::make_shared<Mesh>("cylinder", FixPath("../../Assets/cylinder.obj").c_str(), device, commandList));

	// helix
	meshes.push_back(std::make_shared<Mesh>("helix", FixPath("../../Assets/helix.obj").c_str(), device, commandList));

	// sphere
	meshes.push_back(std::make_shared<Mesh>("sphere", FixPath("../../Assets/sphere.obj").c_str(), device, commandList));

	// torus
	meshes.push_back(std::make_shared<Mesh>("torus", FixPath("../../Assets/torus.obj").c_str(), device, commandList));
}

void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f)},
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f)},
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	CreateLights();

	CreateAssets();

	// Meshes create their own BLAS's; we just need to create the TLAS for the scene here
	RaytracingHelper::GetInstance().CreateTopLevelAccelerationStructureForScene(entities);

	// Create the two buffers
	vertexBuffer = DX12Helper::GetInstance().CreateStaticBuffer(sizeof(Vertex), ARRAYSIZE(vertices), vertices);
	indexBuffer = DX12Helper::GetInstance().CreateStaticBuffer(sizeof(unsigned int), ARRAYSIZE(indices), indices);

	// Set up the views
	vbView.StrideInBytes = sizeof(Vertex);
	vbView.SizeInBytes = sizeof(Vertex) * ARRAYSIZE(vertices);
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R32_UINT;
	ibView.SizeInBytes = sizeof(unsigned int) * ARRAYSIZE(indices);
	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
}

void Game::CreateAssets()
{
	// don't have to access the mesh vector directly! use FindMesh() helper function

	entities.push_back(std::make_shared<GameEntity>("leftCylinder", FindMesh("cylinder"), woodMaterial));
	FindEntity("leftCylinder")->GetTransform()->SetPosition(XMFLOAT3(-7.0f, 3.0f, 0.0f));

	entities.push_back(std::make_shared<GameEntity>("rightCylinder", FindMesh("cylinder"), bronzeMaterial));
	FindEntity("rightCylinder")->GetTransform()->SetPosition(XMFLOAT3(7.0f, 3.0f, 0.0f));

	entities.push_back(std::make_shared<GameEntity>("centerTorus", FindMesh("torus"), bronzeMaterial));
	FindEntity("centerTorus")->GetTransform()->SetPosition(XMFLOAT3(0.0f, -1.0f, 0.0f));
	FindEntity("centerTorus")->GetTransform()->Scale(XMFLOAT3(2.0f, 2.0f, 2.0f));

	entities.push_back(std::make_shared<GameEntity>("leftHelix", FindMesh("helix"), paintMaterial));
	FindEntity("leftHelix")->GetTransform()->SetPosition(XMFLOAT3(-7.0f, 0.0f, 0.0f));

	entities.push_back(std::make_shared<GameEntity>("rightHelix", FindMesh("helix"), woodMaterial));
	FindEntity("rightHelix")->GetTransform()->SetPosition(XMFLOAT3(7.0f, 0.0f, 0.0f));


	for (int i = 0; i < NUM_SPHERES; i++) {
		std::string sName = "sphere" + i;
		std::shared_ptr<Material> sMaterial;

		if (i % 2) {
			sMaterial = bronzeMaterial;
		}
		else {
			sMaterial = paintMaterial;
		}

		entities.push_back(std::make_shared<GameEntity>(sName, FindMesh("sphere"), sMaterial));
		FindEntity(sName)->GetTransform()->SetPosition(XMFLOAT3((rand() % 10) - 5, (rand() % 10) - 4, 0.0f));
	}

	// floor
	entities.push_back(std::make_shared<GameEntity>("floor", FindMesh("cube"), woodMaterial));
	FindEntity("floor")->GetTransform()->SetPosition(XMFLOAT3(0.0f, -6.0f, 0.0f));
	FindEntity("floor")->GetTransform()->Scale(XMFLOAT3(15.0f, 1.0f, 15.0f));
}

void Game::LoadMaterials()
{
	//DX12Helper::GetInstance();

	// make 2 materials, each has 4 textures:
	// - albedo
	// - metal
	// - normal
	// - roughness

	// material constructor expects pipeline state, float3 colortint, float2 uvscale, float2 uvoffset

	// bronze
	bronzeMaterial = std::make_shared<Material>(pipelineState, DirectX::XMFLOAT3(1, .5, 0));

	{
		// bronze textures
		D3D12_CPU_DESCRIPTOR_HANDLE bAlbedo = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/bronze_albedo.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE bMetal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/bronze_metal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE bNormal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/bronze_normal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE bRough = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/bronze_roughness.png").c_str());

		// giving textures to materials
		bronzeMaterial->AddTexture(bAlbedo, 0);
		bronzeMaterial->AddTexture(bMetal, 1);
		bronzeMaterial->AddTexture(bNormal, 2);
		bronzeMaterial->AddTexture(bRough, 3);
	}

	// finalize the material
	bronzeMaterial->FinalizeMaterial();


	// wood
	woodMaterial = std::make_shared<Material>(pipelineState, DirectX::XMFLOAT3(1, .3, .3));

	{
		// wood textures
		D3D12_CPU_DESCRIPTOR_HANDLE wAlbedo = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/wood_albedo.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE wMetal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/wood_metal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE wNormal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/wood_normal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE wRough = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/wood_roughness.png").c_str());

		// giving textures to materials
		woodMaterial->AddTexture(wAlbedo, 0);
		woodMaterial->AddTexture(wMetal, 1);
		woodMaterial->AddTexture(wNormal, 2);
		woodMaterial->AddTexture(wRough, 3);
	}

	// finalize the material
	woodMaterial->FinalizeMaterial();


	// paint
	paintMaterial = std::make_shared<Material>(pipelineState, XMFLOAT3(.6, .3, .6));

	{
		// paint textures
		D3D12_CPU_DESCRIPTOR_HANDLE pAlbedo = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/paint_albedo.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE pMetal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/paint_metal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE pNormal = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/paint_normal.png").c_str());
		D3D12_CPU_DESCRIPTOR_HANDLE pRough = DX12Helper::GetInstance().LoadTexture(FixPath(L"../../Assets/Textures/PBR/paint_roughness.png").c_str());

		// giving textures to materials
		paintMaterial->AddTexture(pAlbedo, 0);
		paintMaterial->AddTexture(pMetal, 1);
		paintMaterial->AddTexture(pNormal, 2);
		paintMaterial->AddTexture(pRough, 3);
	}

	// finalize the material
	paintMaterial->FinalizeMaterial();
}

void Game::CreateCamera()
{
	camera = std::make_shared<Camera>(
		0.0f, 0.0f, -15.0f,
		5.0f,
		1.0f,
		XM_PIDIV4, // pi/4
		float(this->windowWidth) / this->windowHeight
	);
}

void Game::CreateLights()
{
	// 3 directional lights, 2 point lights = 5 total lights

	// directional lights

	// 1: primary light source
	Light directionalLight1 = {};
	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(0, -.75, 0);
	directionalLight1.Intensity = 1.0f;
	directionalLight1.Color = XMFLOAT3(1, 1, 1);

	// 2
	Light directionalLight2 = {};
	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(1, 0, 0);
	directionalLight2.Intensity = 1.0f;
	directionalLight2.Color = XMFLOAT3(1.0f, 0.3f, 0.3f);

	// 3
	Light directionalLight3 = {};
	directionalLight3.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = XMFLOAT3(0, 0, 1);
	directionalLight3.Intensity = 1.0f;
	directionalLight3.Color = XMFLOAT3(1, 1, 1);


	// point lights

	// 4
	Light pointLight1 = {};
	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Range;
	pointLight1.Intensity = 1.0f;
	pointLight1.Color = XMFLOAT3(0.7f, 0.1f, 0.7f); // purple

	// 5
	Light pointLight2 = {};
	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Range;
	pointLight2.Intensity = 1.0f;
	pointLight2.Color = XMFLOAT3(1.0f, 0.4f, 0.7f); // pink?


	// adding lights to list
	lights.push_back(directionalLight1);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);

	// make sure list is the right size (thank you Chris this is so cool)
	lights.resize(TOTAL_LIGHTS);
}

// --------------------------------------------------------
// Loads the two basic shaders, then creates the root signature
// and pipeline state object for our very basic demo.
// --------------------------------------------------------
void Game::CreateRootSigAndPipelineState()
{
	// Blobs to hold raw shader byte code used in several steps below
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;

	// texture2D set up
	{
		D3D12_DESCRIPTOR_RANGE texture2DRange{};
		texture2DRange.BaseShaderRegister = 0;
		texture2DRange.NumDescriptors = 4 ;
		texture2DRange.OffsetInDescriptorsFromTableStart = 0;
		texture2DRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texture2DRange.RegisterSpace = 0;
		//texture2DRange.RegisterSpace = myTex2DSpace;

		D3D12_ROOT_PARAMETER texture2DTable{};
		texture2DTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		texture2DTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		texture2DTable.DescriptorTable.NumDescriptorRanges = 1;
		texture2DTable.DescriptorTable.pDescriptorRanges = &texture2DRange;
	}

	// Load shaders
	{
		// Read our compiled vertex shader code into a blob
		// - Essentially just "open the file and plop its contents here"
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(),
			vertexShaderByteCode.GetAddressOf());
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(),
			pixelShaderByteCode.GetAddressOf());
	}

	// Input layout
	const unsigned int inputElementCount = 4;
	D3D12_INPUT_ELEMENT_DESC inputElements[inputElementCount] = {};
	{
		// Create an input layout that describes the vertex format
		// used by the vertex shader we're using
		// - This is used by the pipeline to know how to interpret the raw data
		// sitting inside a vertex buffer
		// Set up the first element - a position, which is 3 float values
		inputElements[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[0].SemanticName = "POSITION";
		inputElements[0].SemanticIndex = 0;

		// Set up the second element - UV
		inputElements[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElements[1].SemanticName = "TEXTCOORD";
		inputElements[1].SemanticIndex = 0;

		// Set up the third element - NORMAL
		inputElements[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[2].SemanticName = "NORMAL";
		inputElements[2].SemanticIndex = 0;

		// Set up the four element - TANGENT
		inputElements[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElements[3].SemanticName = "TANGENT";
		inputElements[3].SemanticIndex = 0;
	}

	// Root Signature
	{
		D3D12_DESCRIPTOR_RANGE texture2DRange{};
		texture2DRange.BaseShaderRegister = 0;
		texture2DRange.NumDescriptors = 4;
		texture2DRange.OffsetInDescriptorsFromTableStart = 0;
		texture2DRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		texture2DRange.RegisterSpace = 0;

		D3D12_ROOT_PARAMETER texture2DTable{};
		texture2DTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		texture2DTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		texture2DTable.DescriptorTable.NumDescriptorRanges = 1;
		texture2DTable.DescriptorTable.pDescriptorRanges = &texture2DRange;

		// Describe the range of CBVs needed for the vertex shader
		D3D12_DESCRIPTOR_RANGE cbvRangeVS = {};
		cbvRangeVS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangeVS.NumDescriptors = 1;
		cbvRangeVS.BaseShaderRegister = 0;
		cbvRangeVS.RegisterSpace = 0;
		cbvRangeVS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Describe the range of CBVs needed for the pixel shader
		D3D12_DESCRIPTOR_RANGE cbvRangePS = {};
		cbvRangePS.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		cbvRangePS.NumDescriptors = 1;
		cbvRangePS.BaseShaderRegister = 0;
		cbvRangePS.RegisterSpace = 0;
		cbvRangePS.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create a range of SRV's for textures
		D3D12_DESCRIPTOR_RANGE srvRange = {};
		srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		srvRange.NumDescriptors = 4; // Set to max number of textures at once (match pixel shader!)
		srvRange.BaseShaderRegister = 0; // Starts at s0 (match pixel shader!)
		srvRange.RegisterSpace = 0;
		srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// Create the root parameters
		D3D12_ROOT_PARAMETER rootParams[3] = {};

		// CBV table param for vertex shader
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[0].DescriptorTable.pDescriptorRanges = &cbvRangeVS;

		// CBV table param for pixel shader
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[1].DescriptorTable.pDescriptorRanges = &cbvRangePS;

		// SRV table param
		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[2].DescriptorTable.pDescriptorRanges = &srvRange;

		// Create a single static sampler (available to all pixel shaders at the same slot)
		D3D12_STATIC_SAMPLER_DESC anisoWrap = {};
		anisoWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		anisoWrap.Filter = D3D12_FILTER_ANISOTROPIC;
		anisoWrap.MaxAnisotropy = 16;
		anisoWrap.MaxLOD = D3D12_FLOAT32_MAX;
		anisoWrap.ShaderRegister = 0; // register(s0)
		anisoWrap.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC samplers[] = { anisoWrap };

		// Describe and serialize the root signature
		D3D12_ROOT_SIGNATURE_DESC rootSig = {};
		rootSig.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSig.NumParameters = ARRAYSIZE(rootParams);
		rootSig.pParameters = rootParams;
		rootSig.NumStaticSamplers = ARRAYSIZE(samplers);
		rootSig.pStaticSamplers = samplers;

		ID3DBlob* serializedRootSig = 0;
		ID3DBlob* errors = 0;
		D3D12SerializeRootSignature(
			&rootSig,
			D3D_ROOT_SIGNATURE_VERSION_1,
			&serializedRootSig,
			&errors);

		// Check for errors during serialization
		if (errors != 0)
		{
			OutputDebugString((wchar_t*)errors->GetBufferPointer());
		}

		// Actually create the root sig
		device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(rootSignature.GetAddressOf()));
	}

	// Pipeline state
	{
		// Describe the pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		// -- Input assembler related ---
		psoDesc.InputLayout.NumElements = inputElementCount;
		psoDesc.InputLayout.pInputElementDescs = inputElements;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Root sig
		psoDesc.pRootSignature = rootSignature.Get();

		// -- Shaders (VS/PS) ---
		psoDesc.VS.pShaderBytecode = vertexShaderByteCode->GetBufferPointer();
		psoDesc.VS.BytecodeLength = vertexShaderByteCode->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pixelShaderByteCode->GetBufferPointer();
		psoDesc.PS.BytecodeLength = pixelShaderByteCode->GetBufferSize();

		// -- Render targets ---
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;

		// -- States ---
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.DepthStencilState.DepthEnable = true;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask =
			D3D12_COLOR_WRITE_ENABLE_ALL;

		// -- Misc ---
		psoDesc.SampleMask = 0xffffffff;

		// Create the pipe state object
		device->CreateGraphicsPipelineState(&psoDesc,
			IID_PPV_ARGS(pipelineState.GetAddressOf()));
	}
}

// looks through list of available meshes and finds the one that matches the name inserted
std::shared_ptr<Mesh> Game::FindMesh(std::string meshName)
{
	std::shared_ptr<Mesh> myMesh = {};

	for (auto& m : meshes) {
		if (m->name == meshName) {
			myMesh = m;
		}
	}

	return myMesh;
}

// looks through list of created entities and finds the one that matches
std::shared_ptr<GameEntity> Game::FindEntity(std::string entityName)
{
	std::shared_ptr<GameEntity> myEntity = {};

	for (auto& e : entities) {
		if (e->name == entityName) {
			myEntity = e;
		}
	}

	return myEntity;
}

#pragma endregion

// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	camera->UpdateProjectionMatrix(camera->GetFOV(), camera->GetAspectRatio());
	RaytracingHelper::GetInstance().ResizeOutputUAV(windowWidth, windowHeight);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// updating the camera
	camera->Update(deltaTime);

	// moving objects
	FindEntity("centerTorus")->GetTransform()->Rotate(2.0f * deltaTime, -2.0f * deltaTime, 0.0f);

	FindEntity("leftCylinder")->GetTransform()->Rotate(-2.0f * deltaTime, -2.0f * deltaTime, 0.0f);
	FindEntity("rightCylinder")->GetTransform()->Rotate(2.0f * deltaTime, 2.0f * deltaTime, 0.0f);

	FindEntity("leftHelix")->GetTransform()->Rotate(0.0f, 2.0f * deltaTime, 0.0f);
	FindEntity("rightHelix")->GetTransform()->Rotate(0.0f, 2.0f * deltaTime, 0.0f);

	for (int i = 0; i < NUM_SPHERES; i++) {
		std::string sName = "sphere" + i;

		XMFLOAT3 pos = FindEntity(sName)->GetTransform()->GetPosition();

		pos.x = sin((totalTime + i) * 0.7f) * 5;
		pos.z = sin((totalTime + i) * 0.3f) * 5;

		FindEntity(sName)->GetTransform()->SetPosition(pos);
	}

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	// Rendering here!
	{
		// helper variable to make things easy :)
		DX12Helper& dx12Helper = DX12Helper::GetInstance();

		// reset command allocator
		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), 0);

		// Grab the current back buffer for this frame
		Microsoft::WRL::ComPtr<ID3D12Resource> currentBackBuffer = backBuffers[currentSwapBuffer];

		// Update raytracing accel structure
		RaytracingHelper::GetInstance().
			CreateTopLevelAccelerationStructureForScene(entities);

		// Perform raytrace, including execution of command list
		RaytracingHelper::GetInstance().Raytrace(
			camera,
			backBuffers[currentSwapBuffer]);

		// Present the current back buffer
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Figure out which buffer is next
		currentSwapBuffer++;
		if (currentSwapBuffer >= numBackBuffers)
			currentSwapBuffer = 0;

		DX12Helper::GetInstance().WaitForGPU();

	}

}