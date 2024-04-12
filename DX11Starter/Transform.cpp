#include "Transform.h"
#include <DirectXMath.h>

using namespace DirectX;

// constructor
Transform::Transform() : 
	scale(1, 1, 1),
	pitchYawRoll(0, 0, 0),
	position(0, 0, 0),
	forward(0, 0, 1),
	right(1, 0, 0),
	up(0, 1, 0),
	matrixIsDirty(false),
	vectorsDirty(false)
{
	// transformation values
	SetScale(1, 1, 1);

	// matrices
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

// **** transformations: floats ****

// rotates vector to be absolute/relaive to the world
void Transform::MoveAbsolute(float x, float y, float z)
{
	// making a vector with the parameters
	XMFLOAT3 offset = XMFLOAT3(x, y, z);

	// loading position into math vector
	XMVECTOR posVec = XMLoadFloat3(&position);

	// loading offset into math vector
	XMVECTOR offVec = XMLoadFloat3(&offset);

	// math time!
	posVec = XMVectorAdd(posVec, offVec);

	// copying value back to storage type
	XMStoreFloat3(&position, posVec);

	matrixIsDirty = true;

}

// rotates vector to be relative to me (the object)
void Transform::MoveRelative(float x, float y, float z)
{
	// rotate the vector I'm moving along to be relative to me
	// create a direction vector from the input
	// and rotate to match our current orientation
	XMVECTOR movement = XMVectorSet(x, y, z, 0);
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// the non relative direction for our movement
	XMVECTOR relativeDir = XMVector3Rotate(movement, rotQuat);

	// add and store the results
	XMStoreFloat3(&position, XMLoadFloat3(&position) + relativeDir); // fix this
	matrixIsDirty = true;
}

void Transform::Rotate(float p, float y, float r)
{
	// making a vector with the parameters
	XMFLOAT3 rotationChange = XMFLOAT3(p, y, r);

	// loading rotation into math vector
	XMVECTOR rotVec = XMLoadFloat3(&pitchYawRoll);

	// loading change into math vector
	XMVECTOR rotChangeVec = XMLoadFloat3(&rotationChange);

	// math time!
	rotVec = XMVectorAdd(rotVec, rotChangeVec);

	// copying value back to storage type
	XMStoreFloat3(&pitchYawRoll, rotVec);

	matrixIsDirty = true;
	vectorsDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	Scale(XMFLOAT3(x, y, z));
	matrixIsDirty = true;

}

// **** transformations: vectors **** 

// rotates vector to be absolute/relaive to the world
void Transform::MoveAbsolute(XMFLOAT3 _offset)
{
	// loading position into math vector
	XMVECTOR posVec = XMLoadFloat3(&position);

	// loading offset into math vector
	XMVECTOR offVec = XMLoadFloat3(&_offset);

	// math time!
	posVec = XMVectorAdd(posVec, offVec);

	// copying value back to storage type
	XMStoreFloat3(&position, posVec);

	matrixIsDirty = true;
}

void Transform::Rotate(XMFLOAT3 _rotation)
{
	// loading rotation into math vector
	XMVECTOR rotVec = XMLoadFloat3(&pitchYawRoll);

	// loading change into math vector
	XMVECTOR rotChangeVec = XMLoadFloat3(&_rotation);

	// math time!
	rotVec = XMVectorAdd(rotVec, rotChangeVec);

	// copying value back to storage type
	XMStoreFloat3(&pitchYawRoll, rotChangeVec);

	matrixIsDirty = true;
}

void Transform::Scale(XMFLOAT3 _scale)
{
	// loading scale into math vector
	XMVECTOR scaleVec = XMLoadFloat3(&scale);

	// loading scaling into math vector
	XMVECTOR scaleChangeVec = XMLoadFloat3(&_scale);

	// math time!!
	scaleVec = XMVectorMultiply(scaleVec, scaleChangeVec);

	// copying value back to storage type
	XMStoreFloat3(&scale, scaleVec);

	matrixIsDirty = true;
}

// **** setters: floats ****

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	matrixIsDirty = true;
}

void Transform::SetRotation(float x, float y, float z)
{
	pitchYawRoll = XMFLOAT3(x, y, z);
	matrixIsDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	matrixIsDirty = true;
}

// **** setters: vectors ****

void Transform::SetPosition(XMFLOAT3 _position)
{
	position = _position;
	matrixIsDirty = true;
}

void Transform::SetRotation(XMFLOAT3 _rotation)
{
	pitchYawRoll = _rotation;
	matrixIsDirty = true;
	vectorsDirty = true;
}

void Transform::SetScale(XMFLOAT3 _scale)
{
	scale = _scale;
	matrixIsDirty = true;
}

// **** getters ****

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT3 Transform::GetForward()
{
	// update vectors
	UpdateVectors();
	// then return vectors
	return forward;
}

XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();

	return right;
}

XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();

	return up;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (matrixIsDirty) {

		UpdateMatrices();
	}

	matrixIsDirty = false;
	return world;

}

DirectX::XMFLOAT4X4 Transform::GetWorldInvTranspose()
{
	return worldInverseTranspose;
}

// **** helpers ****

void Transform::UpdateMatrices()
{
	// translation matrix
	XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);

	// rotation matrix
	XMMATRIX r = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));

	// scale matrix
	XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&scale));	

	// multiplying matricies
	XMMATRIX _world = s * r * t;

	// storing the matrix
	XMStoreFloat4x4(&world, _world);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(_world)));

}

void Transform::UpdateVectors()
{
	if (!vectorsDirty)
		return;

	// update the vectors!
	// quaternion math :0
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll)); // rotation quaternion
	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotQuat));
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotQuat));	// starts at 0, 0, 1 instead of moving to one spot and moving from there to another; 
																											// starts from the beginning and moves to new spot each frame

	// we're clean
	vectorsDirty = false;
}
