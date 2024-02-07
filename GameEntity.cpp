#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh_ptr)
{
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
    return std::shared_ptr<Mesh>();
}

void GameEntity::SetMesh()
{
}

std::shared_ptr<Transform> GameEntity::GetTransform()
{
    return std::shared_ptr<Transform>();
}
