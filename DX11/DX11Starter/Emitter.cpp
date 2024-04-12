#include "Emitter.h"

/* only updates emitter related particle data
* - number of particles
* - age of particles
* - position of first living and first dead particles in array
* - particle position (emission)
* particle specific data is updated in particle shaders :)
*/

Emitter::Emitter(Microsoft::WRL::ComPtr<ID3D11Device> _device, std::shared_ptr<Material> _material, int _maxParticles, float _maxLifeTime, int _particlesPerSecond, DirectX::XMFLOAT3 _position)
{
	this->myTransform = std::make_shared<Transform>();

	this->maxParticles = _maxParticles;

	this->device = _device;

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
	while (timeSinceLastEmission > timeBetweenParticles) {
		// call emit function
		Emit(currentTime);
		// update timeSinceLastEmission
		timeSinceLastEmission -= timeBetweenParticles;
	}
}

void Emitter::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	std::shared_ptr<Camera> camera,
	float currentTime)
{
	CopyParticlesToGPU(context);

	// buffer setup
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullBuffer = 0;
	context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	material->PrepareMaterial(myTransform, camera);

	// Vertex data
	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	vs->SetMatrix4x4("view", camera->GetView());
	vs->SetMatrix4x4("projection", camera->GetProjection());
	vs->SetFloat("startSize", 2.0f);
	vs->SetFloat("endSize", 1.0f);
	vs->SetFloat("lifetime", maxLifeTime);
	vs->SetFloat("currentTime", currentTime);

	vs->CopyAllBufferData();

	vs->SetShaderResourceView("ParticleData", particleDataSRV);

	context->DrawIndexed(numAlive * 6, 0, 0);
}

std::shared_ptr<Transform> Emitter::GetTransform()
{
	return myTransform;
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
	particles[currentIndex].position.x += .5 * .7;
	particles[currentIndex].position.y += .8* .7;
	particles[currentIndex].position.z += .4 * .7;


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

	// reset indexBuffer
	indexBuffer.Reset();

	// reset particleBuffer
	particleDataBuffer.Reset();

	// reset particle SRV
	particleDataSRV.Reset();


	// set up particle array
	particles = new ParticleData[maxParticles];
	//ZeroMemory(particles, sizeof(Particle) * maxParticles);

	// create index buffer to draw particles
	unsigned int* indices = new unsigned int[maxParticles * 6];
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

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Structured Buffer(s) for particles
	{
		// index buffer!
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;
		ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
		device->CreateBuffer(&ibDesc, &indexData, indexBuffer.GetAddressOf());
		delete[] indices;

		// Make a dynamic buffer to hold all particle data on GPU
		// Note: We'll be overwriting this every frame with new lifetime data
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(ParticleData);
		desc.ByteWidth = sizeof(ParticleData) * maxParticles;
		device->CreateBuffer(&desc, 0, particleDataBuffer.GetAddressOf());

		// SRV!
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = maxParticles;
		device->CreateShaderResourceView(particleDataBuffer.Get(), &srvDesc, particleDataSRV.GetAddressOf());
	}
}

void Emitter::CopyParticlesToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	// Map the buffer
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(particleDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	// How are living particles arranged in the buffer?
	if (firstAlive < firstDead)
	{
		// Only copy from FirstAlive -> FirstDead
		memcpy(
			mappedBuffer.pData, // Destination = start of particle buffer
			particles + firstAlive, // Source = particle array, offset to first living particle
			sizeof(ParticleData) * numAlive); // Amount = number of particles (measured in BYTES!)
	}
	else
	{
		// Copy from 0 -> FirstDead
		memcpy(
			mappedBuffer.pData, // Destination = start of particle buffer
			particles, // Source = start of particle array
			sizeof(ParticleData) * firstDead); // Amount = particles up to first dead (measured in BYTES!)
		// ALSO copy from FirstAlive -> End
		memcpy(
			(void*)((ParticleData*)mappedBuffer.pData + firstDead), // Destination = particle buffer, AFTER the data we copied in previous memcpy()
			particles + firstAlive, // Source = particle array, offset to first living particle
			sizeof(ParticleData) * (maxParticles - firstAlive)); // Amount = number of living particles at end of array (measured in BYTES!)
	}

	context->Unmap(particleDataBuffer.Get(), 0);
}

