#pragma once

#include "Vertex.h"
#include "DX12Helper.h"

#include <d3d11.h> // for referencing Direct3D stuff
#include <wrl/client.h> // when using ComPtrs for Direct3D objects

class Mesh
{
public:

	struct MeshRaytracingData
	{
		D3D12_GPU_DESCRIPTOR_HANDLE IndexbufferSRV{ };
		D3D12_GPU_DESCRIPTOR_HANDLE VertexBufferSRV{ };
		Microsoft::WRL::ComPtr<ID3D12Resource> BLAS;
		unsigned int HitGroupIndex = 0;
	};

	// takes vertices and indices to create mesh
	Mesh(Vertex* _vertices, int numVertices, unsigned int* _indices, int numIndices, Microsoft::WRL::ComPtr<ID3D12Device> _device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList);

	// takes filename to create mesh
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D12Device> _device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList);

	~Mesh();

	// **** getters ****

	// returns vertex buffer pointer
	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer();

	// returns index buffer pointer
	Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer();

	// get buffer views
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	int GetIndexCount();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetComandList();

	// sets buffers; tells DirectX to draw the correct number of indices
	void Draw();

	D3D12_VERTEX_BUFFER_VIEW GetVBView() { return vbView; } // Renamed
	D3D12_INDEX_BUFFER_VIEW GetIBView() { return ibView; } // Renamed
	Microsoft::WRL::ComPtr<ID3D12Resource> GetVBResource() { return vertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetIBResource() { return indexBuffer; }
	MeshRaytracingData GetRaytracingData() { return raytracingData; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	int indices;
	int vertices;

	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INDEX_BUFFER_VIEW ibView;

	// **** helpers ****

	void CreateBuffers(Vertex* _vertices, int numVertices, unsigned int* _indices, int numIndices, Microsoft::WRL::ComPtr<ID3D12Device> _device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	MeshRaytracingData raytracingData;
};

