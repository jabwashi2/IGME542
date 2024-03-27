#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include <iostream>
#include <string>

class GameEntity
{
public:
	// name of this mesh
	std::string name;

	GameEntity(std::string _name, std::shared_ptr<Mesh> mesh_ptr, std::shared_ptr<Material> mtrl_ptr);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	// may not use this
	void SetMesh();
	void SetMaterial(std::shared_ptr<Material> mtrl_ptr);
	std::shared_ptr<Transform> GetTransform();

private:
	std::shared_ptr<Mesh> myMesh;
	std::shared_ptr<Transform> myTransform;
	std::shared_ptr<Material> myMaterial;
};

