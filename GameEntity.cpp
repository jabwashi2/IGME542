#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh_ptr)
{

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
