#include "Mesh.h"
#include "DX12Helper.h"
#include "Vertex.h" // holds our custom Vertex struct
#include "Input.h"
#include "PathHelpers.h"

#include <d3d11.h> // for referencing Direct3D stuff
#include <wrl/client.h> // when using ComPtrs for Direct3D objects
#include <iostream>
#include <fstream>
#include <d3dcompiler.h>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

Mesh::Mesh(Vertex* _vertices, int numVertices, unsigned int* _indices, int numIndices, Microsoft::WRL::ComPtr<ID3D12Device> _device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList)
{
	this->commandList = _commandList;
	this->indices = numIndices;
	this->vertices = numVertices;

	vbView = {};
	ibView = {};

	// call tangent calculating function
	CalculateTangents(_vertices, numVertices, _indices, numIndices);

	// creating buffers
	CreateBuffers(_vertices, numVertices, _indices, numIndices, _device);
}
Mesh::Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D12Device> _device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList)
{
	// Author: Chris Cascioli
	// Purpose: Basic .OBJ 3D model loading, supporting positions, uvs and normals
	// 
	// - You are allowed to directly copy/paste this into your code base
	//   for assignments, given that you clearly cite that this is not
	//   code of your own design.
	//
	// - NOTE: You'll need to #include <fstream>

	this->commandList = _commandList;

	vbView = {};
	ibView = {};

	this->indices = 0;
	this->vertices = 0;

	// File input object
	std::ifstream obj(fileName);
	// Check for successful open
	if (!obj.is_open())
		return;
	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;	// Positions from the file
	std::vector<XMFLOAT3> normals;		// Normals from the file
	std::vector<XMFLOAT2> uvs;		// UVs from the file
	std::vector<Vertex> verts;		// Verts we're assembling
	std::vector<UINT> indices;		// Indices of these verts
	int vertCounter = 0;			// Count of vertices
	int indexCounter = 0;			// Count of indices
	char chars[100];			// String for line reading
	// Still have data left?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);
		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);
			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);
			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);
			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			// NOTE: This assumes the given obj file contains
			//  vertex positions, uv coordinates AND normals.
			unsigned int i[12];
			int numbersRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);
			// If we only got the first number, chances are the OBJ
			// file has no UV coordinates.  This isn't great, but we
			// still want to load the model without crashing, so we
			// need to re-read a different pattern (in which we assume
			// there are no UVs denoted for any of the vertices)
			if (numbersRead == 1)
			{
				// Re-read with a different pattern
				numbersRead = sscanf_s(
					chars,
					"f %d//%d %d//%d %d//%d %d//%d",
					&i[0], &i[2],
					&i[3], &i[5],
					&i[6], &i[8],
					&i[9], &i[11]);
				// The following indices are where the UVs should 
				// have been, so give them a valid value
				i[1] = 1;
				i[4] = 1;
				i[7] = 1;
				i[10] = 1;
				// If we have no UVs, create a single UV coordinate
				// that will be used for all vertices
				if (uvs.size() == 0)
					uvs.push_back(XMFLOAT2(0, 0));
			}
			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];
			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];
			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];
			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)
			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;
			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;
			// Flip normal's Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;
			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);
			vertCounter += 3;
			// Add three more indices
			indices.push_back(indexCounter); indexCounter += 1;
			indices.push_back(indexCounter); indexCounter += 1;
			indices.push_back(indexCounter); indexCounter += 1;
			// Was there a 4th face?
			// - 12 numbers read means 4 faces WITH uv's
			// - 8 numbers read means 4 faces WITHOUT uv's
			if (numbersRead == 12 || numbersRead == 8)
			{
				// Make the last vertex
				Vertex v4;
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];
				// Flip the UV, Z pos and normal's Z
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;
				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);
				vertCounter += 3;
				// Add three more indices
				indices.push_back(indexCounter); indexCounter += 1;
				indices.push_back(indexCounter); indexCounter += 1;
				indices.push_back(indexCounter); indexCounter += 1;
			}
		}
	}
	// Close the file and create the actual buffers
	obj.close();
	
	this->indices = indexCounter;
	this->vertices = vertCounter;

	// call tangent calculating function
	CalculateTangents(&verts[0], vertCounter, &indices[0], indexCounter);

	// creating buffers
	CreateBuffers(&verts[0], vertCounter, &indices[0], indexCounter, _device);

}
Mesh::~Mesh()
{
	// nothing for now!
}

// **** getters ****

Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}
D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBufferView()
{
	return vbView;
}
D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
	return ibView;
}
int Mesh::GetIndexCount()
{
	return indices;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Mesh::GetComandList()
{
	return commandList;
}


void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	{
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);
		commandList->DrawIndexedInstanced(indices, 1, 0, 0, 0);
	}
}

// **** helpers ****

void Mesh::CreateBuffers(Vertex* _vertices, int numVertices, unsigned int* _indices, int numIndices, Microsoft::WRL::ComPtr<ID3D12Device> _device)
{
	DX12Helper& dx12Helper = DX12Helper::GetInstance();

	// creating the vertex buffer
	vertexBuffer = dx12Helper.CreateStaticBuffer(sizeof(Vertex), numVertices, _vertices);
	
	// creating the index buffer
	indexBuffer = dx12Helper.CreateStaticBuffer(sizeof(unsigned int), numIndices, _indices);

	// Set up the views
	vbView.StrideInBytes = sizeof(Vertex);
	vbView.SizeInBytes = sizeof(Vertex) * numVertices;
	vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();

	ibView.Format = DXGI_FORMAT_R32_UINT;
	ibView.SizeInBytes = sizeof(unsigned int) * numIndices;
	ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();	
}
// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Calculates the tangents of the vertices in a mesh
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Code originally adapted from: http://www.terathon.com/code/tangent.html
//   - Updated version now found here: http://foundationsofgameenginedev.com/FGED2-sample.pdf
//   - See listing 7.4 in section 7.5 (page 9 of the PDF)
//
// - Note: For this code to work, your Vertex format must
//         contain an XMFLOAT3 called Tangent
//
// - Be sure to call this BEFORE creating your D3D vertex/index buffers
// --------------------------------------------------------
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numIndices;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthonormalize to ensure
		// the normal and tangent are exactly 90 degrees apart
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}