#pragma once

#include "Material.h"
#include "Transform.h"

#include <DirectXMath.h>
#include <memory>

class Emitter
{
public:
	// struct for a single particle
	struct ParticleData {

		float emitTime;
		DirectX::XMFLOAT3 position;

	};


	Emitter(std::shared_ptr<Material> _material, int _maxParticles, int _firstAlive, int _firstDead, int _numAlive, float _maxLifeTime, int _particlesPerSecond, int _timeBetweenParticles, int _timeSinceLastEmission);

	// update method (track lifetimes, emit particles)
	void Update(float dt, float currentTime);

	// draw method (CPU->GPU copies, draw emitter)
	void Draw();

private:

	// transform for the emitter (so we can resize and move it)
	std::shared_ptr<Transform> myTransform;

	// max num of particles
	int maxParticles;

	// array of particles
	ParticleData* particles; // pointer to every possible particle

	// int for alive start
	int firstAlive;

	// int for dead start
	int firstDead;

	// int for num particles currently alive
	int numAlive;

	// max lifetime (int or float)
	float maxLifeTime;

	// num particles per second (int)
	int particlesPerSecond;

	// fractional seconds between particles (double or float)
	float timeBetweenParticles;

	// time since last emission (float)
	float timeSinceLastEmission;

	// GPU bugger of particles (buffer and SRV)


	// index buffer


	// texture/material
	std::shared_ptr<Material> material;

	// shader(s)


	// helper methods
	
};

