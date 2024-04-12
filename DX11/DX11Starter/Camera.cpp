// note: we will be using left handed axes!!! that's our default

#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float x, float y, float z, float moveSpeed, float mouseLookSpeed, float _fov, float _aspectRatio):
	moveSpeed(moveSpeed),
	mouseLookSpeed(mouseLookSpeed) // if we set value = to paramater, can initialize here
{
	transform.SetPosition(x, y, z); // if setting value = to something else, initialize here

	isActive = true;

	fov = _fov;
	aspectRatio = _aspectRatio;

	// set up matrices >:)
	UpdateViewMatrix();
	UpdateProjectionMatrix(fov, aspectRatio);

}

Camera::~Camera()
{
	// nothing for now :)
}

void Camera::Update(float dt)
{
	// we want to use keyboard input to move around
	// grab an instance of input manager
	Input& input = Input::GetInstance();

	// current speed
	float speed = dt * moveSpeed;

	// keyboard controls
	if (input.KeyDown('W')) { transform.MoveRelative(0, 0, speed); } // forward
	if (input.KeyDown('S')) { transform.MoveRelative(0, 0, -speed); } // backwards
	if (input.KeyDown('A')) { transform.MoveRelative(-speed, 0, 0); } // left
	if (input.KeyDown('D')) { transform.MoveRelative(speed, 0, 0); } // right
	if (input.KeyDown(' ')) { transform.MoveRelative(0, speed, 0); } // up
	if (input.KeyDown('X')) { transform.MoveRelative(0, -speed, 0); } // down

	// mouse controls
	if (input.MouseLeftDown()) {
		float xDiff = 0.001f * mouseLookSpeed * input.GetMouseXDelta();
		float yDiff = 0.001f * mouseLookSpeed * input.GetMouseYDelta();
		transform.Rotate(yDiff, xDiff, 0);
	}

	UpdateViewMatrix(); // want to update the view matrix every time the camera's transform updates
}

void Camera::UpdateViewMatrix()
{
	// grab some transform data
	XMFLOAT3 pos = transform.GetPosition();
	XMFLOAT3 fwd = transform.GetForward();

	// how are we defining which way to look?
	// - ex: at a particular thing (lookAt) or in a certain direction (lookTo)?
	XMMATRIX view = XMMatrixLookToLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&fwd),
		XMVectorSet(0, 1, 0, 0)
	);

	XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix(float _fov, float _aspectRatio)
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		_fov,
		_aspectRatio,
		0.01f, // near clip distance; how close can we get until we can't see it?
		1000.0f // far clip distance; how far can we get until we can't see it?
	);
	XMStoreFloat4x4(&projectionMatrix, proj);
}

Transform* Camera::GetTransform()
{
	return &transform;
}

XMFLOAT4X4 Camera::GetView()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return projectionMatrix;
}

float Camera::GetFOV()
{
	return fov;
}

float Camera::GetAspectRatio()
{
	return aspectRatio;
}
