#include "Emitter.h"



Emitter::Emitter(std::shared_ptr<Material> _material, int _maxParticles, int _firstAlive, int _firstDead, int _numAlive, float _maxLifeTime, int _particlesPerSecond, int _timeBetweenParticles, int _timeSinceLastEmission)
{
	this->material = _material;
	this->maxParticles = _maxParticles;
	this->firstAlive = _firstAlive;
	this->firstDead = _firstDead;
	this->numAlive = _numAlive;
	this->maxLifeTime = _maxLifeTime;
	this->particlesPerSecond = _particlesPerSecond;
	this->timeBetweenParticles = _timeBetweenParticles;
	this->timeSinceLastEmission = _timeSinceLastEmission;

	particles = {};
}

void Emitter::Update(float dt, float currentTime)
{
	// loop through the particles array
	// 
	// in loop:
	// - update position (based on movement) >-- could write helper functions for movement
	// - check age 
	// update current first alive (index)
	// update current first dead (index)

	// do we have living particles?
	if (numAlive > 0) {
		for (int i = 0; i >= numAlive; i++) {

			// update position
			particles[i].position.x += 1; // replace with an actual position altering function

			// update age

		}
	}
}

void Emitter::Draw()
{
	// clear render target


	// render


	// present (dx12)

}
