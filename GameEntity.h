#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include <iostream>

class GameEntity
{
public:
	GameEntity(std::shared_ptr<Mesh> mesh_ptr, std::shared_ptr<Material> mtrl_ptr);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	// may not use this
	void SetMesh();
	std::shared_ptr<Transform> GetTransform();

private:
	std::shared_ptr<Mesh> myMesh;
	std::shared_ptr<Transform> myTransform;
	std::shared_ptr<Material> myMaterial;
};

