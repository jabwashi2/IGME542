#pragma once

#include "Material.h"
#include "Transform.h"
#include "Camera.h"

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


	Emitter(Microsoft::WRL::ComPtr<ID3D11Device> _device, std::shared_ptr<Material> _material, int _maxParticles, float _maxLifeTime, int _particlesPerSecond, DirectX::XMFLOAT3 _position);

	~Emitter();

	// update method (track lifetimes, emit particles)
	void Update(float dt, float currentTime);

	// draw method (CPU->GPU copies, draw emitter)
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<Camera> camera,
		float currentTime);

	// getters/setters

	std::shared_ptr<Transform> GetTransform();

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

	// device
	Microsoft::WRL::ComPtr<ID3D11Device> device;

	// GPU buffer of particles (buffer and SRV)
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleDataBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleDataSRV;

	// index buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// texture/material
	std::shared_ptr<Material> material;

	// helper methods

	// updates one particle, takes index of particle and time
	void SingleUpdate(float currentTime, int index);

	// emits particles (simulation)
	void Emit(float currentTime);

	// creates particle list and buffers
	void CreateParticlesandBuffers();
	void CopyParticlesToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

