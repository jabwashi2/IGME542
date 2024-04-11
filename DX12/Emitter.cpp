#include "Emitter.h"

/* only updates emitter related particle data
* - number of particles
* - age of particles
* - position of first living and first dead particles in array
* - particle position (emission)
* particle specific data is updated in particle shaders :)
*/

Emitter::Emitter(std::shared_ptr<Material> _material, int _maxParticles, float _maxLifeTime, int _particlesPerSecond, DirectX::XMFLOAT3 _position)
{
	myTransform = std::make_shared<Transform>();

	this->maxParticles = _maxParticles;

	particles = {};

	this->firstAlive = 0;
	this->firstDead = 0;
	this->numAlive = 0;
	this->timeSinceLastEmission = 0.0f;

	this->maxLifeTime = _maxLifeTime;

	this->particlesPerSecond = _particlesPerSecond;
	this->timeBetweenParticles = 1.0f / particlesPerSecond;

	myPosition = _position;

	this->material = _material;

	// we'll also make the GPU resources
}

Emitter::~Emitter()
{
	// Clean up the particle array
	delete[] particles;
}

void Emitter::Update(float dt, float currentTime)
{
	// loop through the particles array
	// 
	// in loop:
	// - check age 
	// update current first alive (index)
	// update current first dead (index)

	// do we have living particles?
	if (numAlive > 0) {
		// if firstAlive < firstDead, no need for wrapping
		if (firstAlive < firstDead) {
			for (int i = firstAlive; i < firstDead; i++) {
				SingleUpdate(currentTime, i);
			}
		}

		// if firstAlive > firstDead, need to wrap around
		if (firstAlive > firstDead) {
			for (int i = firstAlive; i < maxParticles; i++) {
				SingleUpdate(currentTime, i);
			}

			for (int i = 0; i < firstDead; i++) {
				SingleUpdate(currentTime, i);
			}
		}

		if (firstAlive == firstDead) {
			for (int i = 0; i < maxParticles; i++) {
				SingleUpdate(currentTime, i);
			}
		}
	}

	// emit particles!
	// first, update timeSinceLastEmission (add dt)
	timeSinceLastEmission += dt;

	// if timeSinceLastEmission > timebetweenparticles then emit
	if (timeSinceLastEmission > timeBetweenParticles) {
		// call emit function
		// update timeSinceLastEmission
		timeSinceLastEmission -= timeBetweenParticles;
	}
}

void Emitter::Draw()
{
	// clear render target


	// render


	// present (dx12)

}

void Emitter::SingleUpdate(float currentTime, int index)
{
	// age check!

	float age = currentTime - particles[index].emitTime;

	if (age >= maxLifeTime) {
		// update firstAlive
		firstAlive++;

		// wrap around
		firstAlive %= maxParticles;

		// update numAlive
		numAlive--;
	}
}

