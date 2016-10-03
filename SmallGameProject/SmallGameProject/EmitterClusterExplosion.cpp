#include "EmitterClusterExplosion.h"



EmitterClusterExplosion::EmitterClusterExplosion()
{
}


EmitterClusterExplosion::~EmitterClusterExplosion()
{
}


void EmitterClusterExplosion::ShutdownSpecific()
{
	this->currentParticleCnt = 0;
	this->particles.clear();
}

bool EmitterClusterExplosion::Initialize(ID3D11Device * device, ID3D11ShaderResourceView * texture, float timeLimit, float spawnRadius, int particleCount)
{
	bool result = false;
	//Set the texture
	this->texture = texture;
	this->world = DirectX::XMMatrixIdentity();
	this->emitterTime = timeLimit;
	this->maxParticles = particleCount;

	//Initialize the emitter
	result = this->InitializeEmitter(spawningRadius);
	if (!result)
		return false;

	//Initialize the buffers
	result = this->InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

bool EmitterClusterExplosion::UpdateSpecific(float dT, ID3D11DeviceContext * deviceContext)
{
	bool result = false;
	this->accumulatedTime /= 1000000;

	//Release old particles
	this->KillParticles();

	//Update the particles
	this->UpdateParticles(this->accumulatedTime);

	//Update the dynamic vertex buffer with the new position of each particle
	result = this->UpdateBuffers(deviceContext);
	if (!result)
		return false;

	return true;
}

void EmitterClusterExplosion::Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters & emitterParameters, int & amountOfParticles)
{
	//NOT IMPLEMENTED
	this->RenderBuffers(deviceContext);

	ParticleShaderParameters parameters;

	parameters.worldMatrix = this->world;
	parameters.diffTexture = this->texture;
	amountOfParticles = this->currentParticleCnt;
}

ID3D11ShaderResourceView * EmitterClusterExplosion::GetTexture()
{
	return this->texture;
}

int EmitterClusterExplosion::GetIndexCount()
{
	return this->indexCount;
}

bool EmitterClusterExplosion::SortParticles()
{
	return true;
}

bool EmitterClusterExplosion::InitializeEmitter(float spawnRadius)
{
	this->velocity = 3.0f;
	this->velocityDeviation = 3.0f;
	this->spawningRadius = spawnRadius;
	this->height = 6.0f;
	this->particleSize = 0.4f;
	this->maxParticles = 100;
	this->isCompleted = false;
	//this->particles.reserve(this->maxParticles);
	this->particles = std::vector<Particle>(this->maxParticles);
	this->currentParticleCnt = 0;
	this->particleTimeLimit = 1.0f;
	this->emitterTime = 2.0f;
	this->emitterTime = 1.0f;
	this->accumulatedTime = 0.0f;

	float positionX, positionY, positionZ, dX, dZ, red, green, blue;
	float particleVelocity = this->velocity;
	positionX = positionY = positionZ = dX = dZ = red = green = blue = 0.0f;
	/*for (auto particle : this->particles)*/
	for (std::vector<Particle>::iterator particle = this->particles.begin(); particle != this->particles.end(); particle++)
	{

		// Now generate the randomized particle properties.
		positionX = 0.0f;
		positionY = this->height;
		positionZ = 0.0f;
		particleVelocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * velocityDeviation;

		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;

		//Calculate the particles dX and dY
		//We want a circle which is 2 PI
		float fullCircle = 2 * Math::MATH_PI;
		float eachRotation = fullCircle / this->maxParticles;
		float myRotation = /*this->currentParticleCnt * */ 1.0f;
		myRotation = (((float)rand() - (float)rand()) / RAND_MAX) * this->maxParticles;
		myRotation *= eachRotation;
		//Define the velocity
		Algorithm::GetCircle(dX, dZ, myRotation);
		//Normalize. Currently turns explosion into a cube
		/*float total = abs(dX) + abs(dZ);
		dZ /= total;
		dX /= total;*/
		//And apply the velocity each will have
		dX *= particleVelocity;
		dZ *= particleVelocity;

		float originDeviation = (((float)rand() - (float)rand()) / RAND_MAX) * this->spawningRadius;

		positionX += dX * originDeviation;
		positionZ += dZ * originDeviation;

		//Define the particle data
		(*particle).x = positionX;
		(*particle).originX = positionX;
		(*particle).y = positionY;
		(*particle).z = positionZ;
		(*particle).originZ = positionZ;
		(*particle).scale = this->particleSize;
		(*particle).r = red;
		(*particle).g = green;
		(*particle).b = blue;
		(*particle).uCoord = 0.25f;
		(*particle).time = 0.0f;
		(*particle).timeCap = this->particleTimeLimit;
		(*particle).dX = dX;
		(*particle).dZ = dZ;
		(*particle).active = true;

		this->currentParticleCnt++;
	}

	return true;
}

bool EmitterClusterExplosion::InitializeBuffers(ID3D11Device * device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	this->vertexCount = this->maxParticles;

	// Set the maximum number of indices in the index array.
	this->indexCount = this->vertexCount;

	// Create the vertex array for the particles that will be rendered.
	this->vertices = new VertexType[this->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[this->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * vertexCount));

	// Initialize the index array.
	for (i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * this->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

void EmitterClusterExplosion::UpdateParticles(float dT)
{
	//Version 2. Possible future experimental parallel version
	Particle_Update up(dT);
	//std::for_each(this->particles.begin(), this->particles.end(), up);
	std::transform(this->particles.begin(), this->particles.end(), this->particles.begin(), up);
	////The parallel version. Wants to be compiled using cl.exe "/EHsc" without the ""
	//parallel_for_each(this->particles.begin(), this->particles.end(), up);
}

void EmitterClusterExplosion::KillParticles()
{
	if (this->accumulatedTime >= this->particleTimeLimit)
	{
		this->particles.clear();
	}
}

bool EmitterClusterExplosion::UpdateBuffers(ID3D11DeviceContext * deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Initialize vertex array to zeros at first.
	memset(this->vertices, 0, (sizeof(VertexType) * this->vertexCount));
	int index = 0;
	for (auto node : this->particles)
	{
		//If the next node doesn't exist, return false. Our particle cnt is wrong
		this->vertices[index].position = DirectX::XMFLOAT4(node.x, node.y, node.z, node.scale);
		this->vertices[index].color = DirectX::XMFLOAT4(node.r, node.g, node.b, node.uCoord);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(this->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)this->vertices, (sizeof(VertexType) * this->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(this->vertexBuffer, 0);

	return true;

}

void EmitterClusterExplosion::RenderBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}