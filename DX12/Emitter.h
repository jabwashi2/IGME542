#pragma once
#include <DirectXMath.h>

class Emitter
{
public:
	// struct for a single particle
	struct ParticleData {

		float emitTime;
		DirectX::XMFLOAT3 position;

	};

	// max num of particles
	int maxParticles;

	// array of particles
	ParticleData* particles; // pointer to every possible particle

	// int for alive start
	int firstAlive;

	// int for dead start
	int firstDead;

	// int for num particles alive
	int numAlive;

	// max lifetime (int or float)
	float maxLifeTime;

	// num particles per second (int)
	int particelsPerSecond;

	// fractional seconds between particles (double or float)
	float timeBetweenParticles;
	
	// time since last emission (float)
	float timeSinceLastEmission;

	// update method (track lifetimes, emit particles)
	void Update();

	// draw method (CPU->GPU copies, draw emitter)
	void Draw();

private:
	// GPU bugger of particles (buffer and SRV)


	// index buffer


	// texture


	// shader(s)


	// helper methods
	
};

