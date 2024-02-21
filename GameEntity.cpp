#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh_ptr, std::shared_ptr<Material> mtrl_ptr)
{
    myMesh = mesh_ptr;
    myTransform = std::make_shared<Transform>();
    myMaterial = mtrl_ptr;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return myMesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial() {
    return myMaterial;
}

// set up if needed
void GameEntity::SetMesh()
{

}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return myTransform;
}
