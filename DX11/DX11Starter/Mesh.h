#pragma once
#include <d3d11.h> // for referencing Direct3D stuff
#include <wrl/client.h> // when using ComPtrs for Direct3D objects
#include "Vertex.h"

class Mesh
{
public:
	// takes vertices and indices to create mesh
	Mesh(Vertex *_vertices, int numVertices, unsigned int *_indices, int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context);

	// takes filename to create mesh
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> _device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context);

	~Mesh();

	// **** getters ****

	// returns vertex buffer pointer
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	// returns index buffer pointer
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext();

	// sets buffers; tells DirectX to draw the correct number of indices
	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	int indices;
	int vertices;

	// **** helpers ****

	void CreateBuffers(Vertex* _vertices, int numVertices, unsigned int* _indices, int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> _device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

