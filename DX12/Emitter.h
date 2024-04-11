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


	Emitter(std::shared_ptr<Material> _material, int _maxParticles, float _maxLifeTime, int _particlesPerSecond, DirectX::XMFLOAT3 _position);

	~Emitter();

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

	// emitter position
	DirectX::XMFLOAT3 myPosition;

	// TODO: note: for D3D12, will need to adjust these things:
	// - particle GPU buffer
	// - particle SRV
	// - render state representation
	//	- new pipeline state object
	//	- includes blend and depth states
	// - resource binding layout
	//	- root sig for particle specific resources
	 
	// TODO: GPU buffer of particles (buffer and SRV)


	// TODO: index buffer
	ID3D12Resource* indexBuffer;

	D3D12_INDEX_BUFFER_VIEW ibv;

	// texture/material
	std::shared_ptr<Material> material;

	// TODO: shader(s)


	// helper methods

	// updates one particle, takes index of particle and time
	void SingleUpdate(float currentTime, int index);

	// emits particles (simulation)
	void Emit(float currentTime);

	// creates particle list and buffers
	void CreateParticlesandBuffers();
};

