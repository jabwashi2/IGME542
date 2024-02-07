#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	// **** transformers ****

	// floats

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void Rotate(float p, float y, float r);
	void Scale(float x, float y, float z);

	// vectors

	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	//void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(DirectX::XMFLOAT3 scale);

	// **** setters ****

	// floats

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	// vectors

	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(DirectX::XMFLOAT3 scale);

	// **** getters ****

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();

	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInvTranspose();

private:
	// raw transformation data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	// local vectors
	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;

	// matrices
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	bool matrixIsDirty;
	bool vectorsDirty;

	void UpdateMatrices();
	void UpdateVectors();
};

