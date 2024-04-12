#include "GameEntity.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Camera.h"

#include <iostream>
#include <DirectXMath.h>

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh_ptr, std::shared_ptr<Material> material_ptr)
{
	myMesh = mesh_ptr;
	myTransform = std::make_shared<Transform>();
	myMaterial = material_ptr;
	editColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.5f, 1.0f);
}

// **** getters ****

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return myMesh;
}
std::shared_ptr<Transform> GameEntity::GetTransform()
{
	return myTransform;
}
std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return myMaterial;
}

void GameEntity::Draw(std::shared_ptr<Camera> camPtr, float totalTime)
{
	// cBuffer
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx = myMesh->GetContext();

	// shader things (constant buffer)
	std::shared_ptr<SimpleVertexShader> vs = myMaterial->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = myMaterial->GetPixelShader();

	myMaterial->Setup();

	ps->SetFloat4("colorTint", myMaterial->GetColorTint());
	ps->SetFloat("totalTime", totalTime);
	ps->SetFloat("roughness", myMaterial->GetRoughness());
	ps->SetFloat3("cameraPosition", camPtr->GetTransform()->GetPosition());

	vs->SetMatrix4x4("world", myTransform->GetWorldMatrix()); // match variable
	vs->SetMatrix4x4("worldInvTranspose", myTransform->GetWorldInvTranspose());
	vs->SetMatrix4x4("view", camPtr->GetView()); // names in your
	vs->SetMatrix4x4("projection", camPtr->GetProjection()); // shader’s cbuffer!

	myMaterial->GetVertexShader()->SetShader();
	myMaterial->GetPixelShader()->SetShader();

	// mapping
	vs->CopyAllBufferData(); // Adjust “vs” variable name if necessary
	ps->CopyAllBufferData(); // Adjust “vs” variable name if necessary

	// setting up vertex + index buffers
	myMesh->Draw();
}

// **** setters ****

void GameEntity::SetMaterial(std::shared_ptr<Material> newMaterial)
{
	myMaterial = newMaterial;
}



