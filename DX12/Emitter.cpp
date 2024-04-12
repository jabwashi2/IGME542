#include "Emitter.h"
#include "DX12Helper.h"

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
	myTransform->SetPosition(myPosition);

	this->material = _material;

	// we'll also make the GPU resources
	CreateParticlesandBuffers();
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
		// TODO: call emit function
		// update timeSinceLastEmission
		timeSinceLastEmission -= timeBetweenParticles;
	}
}

void Emitter::Draw()
{
	// TODO: clear render target


	// TODO: render


	// TODO: present (dx12)

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

void Emitter::Emit(float currentTime)
{
	// make sure there are particles left to spawn
	if (numAlive == maxParticles)
		return;

	// set current index (start at first dead)
	int currentIndex = firstDead;

	// update spawn time
	particles[currentIndex].emitTime = currentTime;

	// TODO: apply particle transformations

	// update firstDead index
	firstDead++;
	firstDead %= maxParticles;

	// update number of living particles
	numAlive++;
}

void Emitter::CreateParticlesandBuffers()
{
	// reset!
	if (particles) {
		delete[] particles;
	}

	// TODO: reset indexBuffer
	// TODO: reset particleBuffer
	// TODO: reset particle SRV

	// set up particle array
	particles = new ParticleData[maxParticles];

	// TODO: create index buffer to draw particles
	int numIndices = maxParticles * 6; // <-- from Chris; two triangles per particle
	unsigned int* indices = new unsigned int[numIndices];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	// TODO: figure this out: D3D11_SUBRESOURCE_DATA indexData = {};
	// indexData.pSysMem = indices;

	// TODO: should we use DX12helper to set these up? probably
	// TODO: set up particleBuffer
	// TODO: set up SRV

	// helper variable to make things easy :)
	DX12Helper& dx12Helper = DX12Helper::GetInstance();

	// Structured Buffer(s) for particles
	{
		// TODO: rewrite for D3D12
		// Make a dynamic buffer to hold all particle data on GPU
		// Note: We'll be overwriting this every frame with new lifetime data
		dx12Helper.CreateStaticBuffer(sizeof(indices[0]), numIndices, indices);

		// Create an SRV that points to a structured buffer of particles
		// so we can grab this data in a vertex shader
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = maxParticles;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.StructureByteStride = sizeof(ParticleData);
		//device->CreateShaderResourceView(particleDataBuffer.Get(), &srvDesc, particleDataSRV.GetAddressOf());
	}
}

