#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh_ptr)
{
    myMesh = mesh_ptr;
    myTransform = std::make_shared<Transform>();
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return myMesh;
}

// set up if needed
void GameEntity::SetMesh()
{

}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return myTransform;
}
