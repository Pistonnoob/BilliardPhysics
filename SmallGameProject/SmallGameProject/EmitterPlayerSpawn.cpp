#include "EmitterPlayerSpawn.h"



EmitterPlayerSpawn::EmitterPlayerSpawn() :
	ParticleEmitter()
{
	this->particleVelocity = 0.0f;
	this->particleSize = this->particlesPerSecond = 0.0f;
}


EmitterPlayerSpawn::~EmitterPlayerSpawn()
{
}

void EmitterPlayerSpawn::ShutdownSpecific()
{
	this->currentParticleCnt = 0;
	this->root.clear();
}

bool EmitterPlayerSpawn::Initialize(ID3D11Device * device, ID3D11ShaderResourceView * texture, float timeLimit)
{
	bool result = false;
	//Set the texture
	this->texture = texture;
	this->world = DirectX::XMMatrixIdentity();
	this->emitterTime = timeLimit;
	this->particleTimeLimit = timeLimit;

	//Initialize the emitter
	result = this->InitializeEmitter();
	if (!result)
		return false;

	//Initialize the buffers
	result = this->InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

bool EmitterPlayerSpawn::UpdateSpecific(float dT, ID3D11DeviceContext * deviceContext)
{
	bool result = false;

	//Release old particles
	this->KillParticles();


	//Emitt new particles
	this->EmittParticles(dT);

	//Update the particles
	this->UpdateParticles(dT);

	//Update the dynamic vertex buffer with the new position of each particle
	result = this->UpdateBuffers(deviceContext);
	if (!result)
		return false;

	return true;
}

void EmitterPlayerSpawn::Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters & emitterParameters, int & amountOfParticles)
{
	//NOT IMPLEMENTED
	this->RenderBuffers(deviceContext);

	ParticleShaderParameters parameters;

	parameters.worldMatrix = this->world;
	parameters.diffTexture = this->texture;
	amountOfParticles = this->currentParticleCnt;
}

ID3D11ShaderResourceView * EmitterPlayerSpawn::GetTexture()
{
	return this->texture;
}

int EmitterPlayerSpawn::GetIndexCount()
{
	return this->indexCount;
}

bool EmitterPlayerSpawn::SortParticles()
{
	return true;
}

bool EmitterPlayerSpawn::InitializeEmitter()
{
	//Particle Velocity
	float totalDistance = Math::MATH_PI * 2 * 2;
	float totalDistancePerSecond = totalDistance / this->emitterTime;
	this->particleVelocity = totalDistancePerSecond;

	this->particleSize = 0.4f;
	this->particlesPerSecond = 10.0f;
	this->maxParticles = 100;
	//this->root = vector<Particle>(this->maxParticles);
	this->root.reserve(this->maxParticles);

	this->currentParticleCnt = 0.0f;

	this->accumulatedTime = 0.0f;

	return true;
}

bool EmitterPlayerSpawn::InitializeBuffers(ID3D11Device * device)
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

void EmitterPlayerSpawn::EmittParticles(float dT)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	// Check if it is time to emit a new particle or not.
	float particleThresshold = (1000000.0f / this->particlesPerSecond);
	float timeOverflow = dT;
	int timeIndex = 0;
 	while (this->accumulatedTime > particleThresshold)
	{
		this->accumulatedTime = this->accumulatedTime - particleThresshold;

		// If there are particles to emit then emit one per frame.
		if (this->currentParticleCnt < this->maxParticles)
		{
			// Now generate the randomized particle properties.
			positionX = 0.0f;
			positionY = 10.0f;
			positionZ = 0.0f;
			velocity = particleVelocity;
			red = 0.0f;
			green = 0.0f;
			blue = 1.0f;

			// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
			// We will sort using Z depth so we need to find where in the list the particle should be inserted.

			found = false;

			Particle toInsert = Particle();
			// Define the particle that we want to insert into our particle list
			toInsert.x = positionX;
			toInsert.y = positionY;
			toInsert.z = positionZ;
			toInsert.scale = this->particleSize;
			toInsert.r = red;
			toInsert.g = green;
			toInsert.b = blue;
			toInsert.uCoord = 0.25f;
			toInsert.time = timeIndex * (1 / this->particlesPerSecond);
			toInsert.timeCap = this->particleTimeLimit;
			toInsert.velocity = velocity;
			toInsert.active = true;

			//If empty root
			if (this->root.size() == 0)
			{
				this->root.push_back(toInsert);
			}
			else
			{
				//Find the appropriate possition and insert the particle there
				int index = 0;
				for (auto node : this->root)
				{
					if (toInsert < node)
					{
						break;
					}
					index++;
				}
				this->root.insert(this->root.begin() + index, toInsert);
			}

			this->currentParticleCnt++;
		}
		timeIndex++;
	}

	return;
}

void EmitterPlayerSpawn::UpdateParticles(float dT)
{
	//float size = 1.0f;
	//float x = 0, y = 0;
	//float period = 8.0f, min = 0, max = 4.0f;
	//float time = 0.0f;
	//float width = 40;
	//int particleCnt = this->currentParticleCnt;
	//for (std::vector<Particle>::iterator node = this->root.begin(); node != this->root.end(); node++)
	//{
	//	x = y = 0;
	//	(*node).time += dT / (1000000);
	//	//node.y = node.y - (node.velocity * dT / 1000);
	//	time = (*node).time * (*node).velocity;

	//	size = (*node).time;

	//	Algorithm::GetEllipse(x, y, time, size, size);

	//	(*node).x = x;
	//	(*node).z = y;
	//}
	//Version 2. Possible future experimental parallel version
	Particle_Update up(dT / 1000000);
	float deltaTime = up.dT;
	//std::for_each(this->root.begin(), this->root.end(), up);
	std::transform(this->root.begin(), this->root.end(), this->root.begin(), up);
	////The parallel version. Wants to be compiled using cl.exe "/EHsc" without the ""
	//parallel_for_each(this->root.begin(), this->root.end(), up);
}

void EmitterPlayerSpawn::KillParticles()
{
	//for (std::vector<Particle>::iterator node = this->root.begin(); node != this->root.end(); node++)
	//{
	//	//If active node, check if it should be deactivated
	//	//if((*node).active)
	//	//	(*node).active = (*node).time < (*node).timeCap;
	//	//Remove deactivated node
	//	if ((*node).time >= (*node).timeCap)
	//		this->root.erase(node);
	//}
	//this->root.erase(std::remove_if(root.begin(), root.end(), my_predicate), root.end());
	this->root.erase(std::remove_if(this->root.begin(), this->root.end(), [](Particle p) { p.active = p.time < p.timeCap; return !p.active;}), this->root.end());
	this->currentParticleCnt = this->root.size();
}

bool EmitterPlayerSpawn::UpdateBuffers(ID3D11DeviceContext * deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Initialize vertex array to zeros at first.
	memset(this->vertices, 0, (sizeof(VertexType) * this->vertexCount));
	int index = 0;
	for (auto node : this->root)
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

void EmitterPlayerSpawn::RenderBuffers(ID3D11DeviceContext * deviceContext)
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
