#include "Sky.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Helpers.h"

using namespace DirectX;

#define LoadShader(type, file) std::make_shared<type>(device.Get(), context.Get(), FixPath(file).c_str())


Sky::Sky(
	const wchar_t* cubemapDDSFile, 
	std::shared_ptr<Mesh> mesh,
	std::shared_ptr<SimpleVertexShader> skyVS, 
	std::shared_ptr<SimplePixelShader> skyPS, 
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions, 
	Microsoft::WRL::ComPtr<ID3D11Device> device, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	// Save params
	this->skyMesh = mesh;
	this->device = device;
	this->context = context;
	this->samplerOptions = samplerOptions;
	this->skyVS = skyVS;
	this->skyPS = skyPS;

	IBLCreateIrradianceMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateConvolvedSpecularMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateBRDFLookUpTexture(); // Use power of 2 for size: 1024 is good here

	// Init render states
	InitRenderStates();

	// Load texture
	CreateDDSTextureFromFile(device.Get(), cubemapDDSFile, 0, skySRV.GetAddressOf());
}

Sky::Sky(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) :
	skySRV(cubeMap),
	samplerOptions(samplerOptions),
	device(device),
	context(context)
{
	IBLCreateIrradianceMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateConvolvedSpecularMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateBRDFLookUpTexture(); // Use power of 2 for size: 1024 is good here

	// Init render states
	InitRenderStates();
}

Sky::Sky(
	const wchar_t* right, 
	const wchar_t* left, 
	const wchar_t* up, 
	const wchar_t* down, 
	const wchar_t* front, 
	const wchar_t* back, 
	std::shared_ptr<Mesh> mesh,
	std::shared_ptr<SimpleVertexShader> skyVS,
	std::shared_ptr<SimplePixelShader> skyPS,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	// Save params
	this->skyMesh = mesh;
	this->device = device;
	this->context = context;
	this->samplerOptions = samplerOptions;
	this->skyVS = skyVS;
	this->skyPS = skyPS;

	IBLCreateIrradianceMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateConvolvedSpecularMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateBRDFLookUpTexture(); // Use power of 2 for size: 1024 is good here

	// Init render states
	InitRenderStates();

	// Create texture from 6 images
	skySRV = CreateCubemap(right, left, up, down, front, back);
}

Sky::Sky(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> right,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> left,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> up,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> down,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> front,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> back,
	std::shared_ptr<Mesh> mesh,
	std::shared_ptr<SimpleVertexShader> skyVS,
	std::shared_ptr<SimplePixelShader> skyPS,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions,
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	// Save params
	this->skyMesh = mesh;
	this->device = device;
	this->context = context;
	this->samplerOptions = samplerOptions;
	this->skyVS = skyVS;
	this->skyPS = skyPS;

	IBLCreateIrradianceMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateConvolvedSpecularMap(); // Use power of 2 for size: 256 or 512 is good
	IBLCreateBRDFLookUpTexture(); // Use power of 2 for size: 1024 is good here

	// Init render states
	InitRenderStates();

	// Create texture from 6 images
	skySRV = CreateCubemap(right, left, up, down, front, back);
}

Sky::~Sky()
{
}

void Sky::Draw(std::shared_ptr<Camera> camera)
{
	// Change to the sky-specific rasterizer state
	context->RSSetState(skyRasterState.Get());
	context->OMSetDepthStencilState(skyDepthState.Get(), 0);

	// Set the sky shaders
	skyVS->SetShader();
	skyPS->SetShader();

	// Give them proper data
	skyVS->SetMatrix4x4("view", camera->GetView());
	skyVS->SetMatrix4x4("projection", camera->GetProjection());
	skyVS->CopyAllBufferData();

	// Send the proper resources to the pixel shader
	skyPS->SetShaderResourceView("skyTexture", skySRV);
	skyPS->SetSamplerState("samplerOptions", samplerOptions);

	// Set mesh buffers and draw
	skyMesh->SetBuffersAndDraw(context);

	// Reset my rasterizer state to the default
	context->RSSetState(0); // Null (or 0) puts back the defaults
	context->OMSetDepthStencilState(0, 0);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::GetIrradianceMap()
{
	return irradianceIBL;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::GetSpecularMap()
{
	return specularIBL;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::GetBRDFLookUpTexture()
{
	return brdfMap;
}

int Sky::GetTotalSpecularIBLMipLevels()
{
	return totalSpecIBLMipLevels;
}

void Sky::InitRenderStates()
{
	// Rasterizer to reverse the cull mode
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT; // Draw the inside instead of the outside!
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rastDesc, skyRasterState.GetAddressOf());

	// Depth state so that we ACCEPT pixels with a depth == 1
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	device->CreateDepthStencilState(&depthDesc, skyDepthState.GetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Specifically NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, 0, textures[0].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), left, 0, textures[1].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), up, 0, textures[2].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), down, 0, textures[3].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), front, 0, textures[4].GetAddressOf());
	CreateWICTextureFromFile(device.Get(), back, 0, textures[5].GetAddressOf());

	// Send back the SRV, which is what we need for our shaders
	return CreateCubemap(textures[0], textures[1], textures[2], textures[3], textures[4], textures[5]);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::CreateCubemap(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> right,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> left,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> up,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> down,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> front,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	ID3D11Resource* textures[6] = {};
	right.Get()->GetResource(&textures[0]);
	left.Get()->GetResource(&textures[1]);
	up.Get()->GetResource(&textures[2]);
	down.Get()->GetResource(&textures[3]);
	front.Get()->GetResource(&textures[4]);
	back.Get()->GetResource(&textures[5]);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	((ID3D11Texture2D*)textures[0])->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array".  This is a special GPU resource format, 
	// NOT just a C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;  // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the actual texture resource
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,	// Which mip (zero, since there's only one)
			i,	// Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture.Get(), // Destination resource
			subresource,		// Dest subresource index (one of the array elements)
			0, 0, 0,			// XYZ location of copy
			textures[i],		// Source resource
			0,					// Source subresource index (we're assuming there's only one)
			0);					// Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;	// Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	// Clean up our extra texture refs
	for (int i = 0; i < 6; i++)
		textures[i]->Release();

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}

void Sky::IBLCreateIrradianceMap(int cubeFaceSize)
{

	Microsoft::WRL::ComPtr<ID3D11Texture2D> irrMapTexture;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = cubeFaceSize;
	texDesc.Height = cubeFaceSize;
	texDesc.ArraySize = 6; // Cube map is 6 textures
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Need both!
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.MipLevels = 1; // No mip chain needed for irradiance map. Specular will need more!
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // It's a cube map, not just an array
	texDesc.SampleDesc.Count = 1; // Can't be zero
	device->CreateTexture2D(&texDesc, 0, irrMapTexture.GetAddressOf()); // irrMapTexture is ComPtr<ID3D11Texture2D>


	// Create an SRV for the irradiance texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this resource as a cube map
	srvDesc.TextureCube.MipLevels = 1; // Only 1 mip level
	srvDesc.TextureCube.MostDetailedMip = 0; // Accessing the first (and only) mip
	srvDesc.Format = texDesc.Format; // Same format as texture
	device->CreateShaderResourceView(irrMapTexture.Get(), &srvDesc, irradianceIBL.GetAddressOf());
	// "irradianceIBL" is ComPtr<ID3D11ShaderResourceView>

	// set up viewport
	unsigned int vpCount = 1;
	D3D11_VIEWPORT prevVP = {};
	context->RSGetViewports(&vpCount, &prevVP);

	// Make sure the viewport matches the texture size
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)cubeFaceSize;
	vp.Height = (float)cubeFaceSize;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	std::shared_ptr<SimpleVertexShader> fullscreenVS = LoadShader(SimpleVertexShader, L"VertexShader.cso");
	std::shared_ptr<SimplePixelShader> irradiancePS = LoadShader(SimplePixelShader, L"IBLIrradianceMapPS");

	fullscreenVS->SetShader();
	irradiancePS->SetShader();
	irradiancePS->SetShaderResourceView("EnvironmentMap", skySRV.Get());
	irradiancePS->SetSamplerState("BasicSampler", samplerOptions.Get());

	// Set states that may or may not be set yet
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < 6; i++) {
		// Make a render target view for this face
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY; // This points to a Texture2D Array
		rtvDesc.Texture2DArray.ArraySize = 1; // How much of the array do we want to access?
		rtvDesc.Texture2DArray.FirstArraySlice = i; // Which array index are we rendering into? 0-5
		rtvDesc.Texture2DArray.MipSlice = 0; // Which mip of that texture are we rendering into?
		rtvDesc.Format = texDesc.Format; // Same format as texture
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		device->CreateRenderTargetView(irrMapTexture.Get(), &rtvDesc, rtv.GetAddressOf());
		// Note: irrMapTexture MUST be created with D3D11_BIND_RENDER_TARGET flag or this fails!
	}

}

void Sky::IBLCreateConvolvedSpecularMap(int cubeFaceSize)
{
	// TODO: finish function
	// create texture on GPU
	// set as current render target
	// change viewport
	// render calculations
	// reset rendering states
}

void Sky::IBLCreateBRDFLookUpTexture(int textureSize)
{
	// TODO: finish function
	// create texture on GPU
	// set as current render target
	// change viewport
	// render calculations
	// reset rendering states
}