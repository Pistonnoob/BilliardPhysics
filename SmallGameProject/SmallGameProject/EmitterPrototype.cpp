#include "EmitterPrototype.h"



EmitterPrototype::EmitterPrototype() : 
	ParticleEmitter()
{
	this->particleDeviationX = this->particleDeviationY = this->particleDeviationZ = 0.0f;
	this->particleVelocity = 0.0f;
	this->particleSize = this->particlesPerSecond = 0.0f;

	this->currentParticleCnt = 0;
	this->maxParticles = 0;
	this->accumulatedTime = 0.0f;
	this->emitterTime = 10.0f;
	this->vertexCount = this->indexCount = 0;
	this->world = DirectX::XMMatrixIdentity();

	this->vertexBuffer = this->indexBuffer = nullptr;

	this->texture = nullptr;
}


EmitterPrototype::~EmitterPrototype()
{
}

void EmitterPrototype::ShutdownSpecific()
{

	if (this->particles)
	{
		delete[] this->particles;
		this->particles = nullptr;
	}
}

bool EmitterPrototype::Initialize(ID3D11Device * device, ID3D11ShaderResourceView * texture, float timeLimit)
{
	bool result = false;
	//Set the texture
	this->texture = texture;
	this->world = DirectX::XMMatrixIdentity();
	this->emitterTime = timeLimit;

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

bool EmitterPrototype::UpdateSpecific(float dT, ID3D11DeviceContext * deviceContext)
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

void EmitterPrototype::Render(ID3D11DeviceContext * deviceContext, ParticleShaderParameters& emitterParameters, int& amountOfParticles)
{
	//NOT IMPLEMENTED
	this->RenderBuffers(deviceContext);

	ParticleShaderParameters parameters;

	parameters.worldMatrix = this->world;
	parameters.diffTexture = this->texture;
	amountOfParticles = this->currentParticleCnt;
}

ID3D11ShaderResourceView * EmitterPrototype::GetTexture()
{
	return this->texture;
}

int EmitterPrototype::GetIndexCount()
{
	return this->indexCount;
}

bool EmitterPrototype::SortParticles()
{
	//Define the distance from the camera to the particle
	for (int i = 0; i < this->currentParticleCnt; i++)
	{
		float cameraDistance = pow(this->particles[i].x - cameraPos.x, 2) + pow(this->particles[i].y - cameraPos.y, 2) + pow(this->particles[i].z - this->cameraPos.z, 2);
		//float cameraDistance = this->particles[i].y;
		//To get the real distance
		//cameraDistance = sqrt(cameraDistance);
		this->particles[i].cameraDistance = cameraDistance;
	}
	//Sort all our particles
	std::sort(this->particles, this->particles + this->currentParticleCnt, by_cameraPos());

	return true;
}


bool EmitterPrototype::InitializeEmitter()
{
	this->particleDeviationX = 20.0f;
	this->particleDeviationY = 0.1f;
	this->particleDeviationZ = 2.0f;

	this->particleVelocity = 1.0f;
	this->particleVelocityVariation = 2.0f;

	this->particleSize = 0.2f;
	this->particlesPerSecond = 4.0f;
	this->maxParticles = 100;

	this->particles = new Particle[this->maxParticles];
	if (!this->particles)
		return false;

	//Initialize particle list
	for (int i = 0; i < this->maxParticles; i++)
	{
		this->particles[i].active = false;
	}

	this->currentParticleCnt = 0.0f;

	this->accumulatedTime = 0.0f;

	return true;
}

bool EmitterPrototype::InitializeBuffers(ID3D11Device * device)
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

void EmitterPrototype::EmittParticles(float dT)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	// Check if it is time to emit a new particle or not.
	float particleThresshold = (1000000.0f / this->particlesPerSecond);
	float timeOverflow = dT;
	while (this->accumulatedTime > particleThresshold)
	{
		this->accumulatedTime = this->accumulatedTime - particleThresshold;
		timeOverflow = timeOverflow - particleThresshold;
		

		// If there are particles to emit then emit one per frame.
		if (this->currentParticleCnt < (this->maxParticles /*- 1*/))
		{

			// Now generate the randomized particle properties.
			positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationX;
			positionY = 20.0f + (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationY;
			positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationZ;
			positionX = 0.0f;
			positionY = 10.0f;
			positionZ = 0.0f;
			velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;
			/*positionY = 20.0f;
			positionZ = 0.0f;
			velocity = particleVelocity;*/
			red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
			green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
			blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
			red = 0.0f;
			green = 0.0f;
			blue = 1.0f;

			// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
			// We will sort using Z depth so we need to find where in the list the particle should be inserted.
			index = this->currentParticleCnt;
			found = false;
			while (!found && index < this->maxParticles)
			{
				if ((this->particles[index].active == false))
					found = true;
				else
					index++;
			}

			// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
			i = this->currentParticleCnt;
			j = i - 1;

			/*while (i != index)
			{
				this->particles[i].x = this->particles[j].x;
				this->particles[i].y = this->particles[j].y;
				this->particles[i].z = this->particles[j].z;
				this->particles[i].r = this->particles[j].r;
				this->particles[i].g = this->particles[j].g;
				this->particles[i].b = this->particles[j].b;
				this->particles[i].velocity = this->particles[j].velocity;
				this->particles[i].active = this->particles[j].active;
				this->particles[i].scale = this->particles[j].scale;
				this->particles[i].rotation = this->particles[j].rotation;
				i--;
				j--;
			}*/

			//Because we will sort the array every time we render we will just set
			//the data at the palce we find

			// Now insert it into the particle array in the correct depth order.
			this->particles[index].x = positionX;
			this->particles[index].y = positionY;
			this->particles[index].z = positionZ;
			this->particles[index].r = red;
			this->particles[index].g = green;
			this->particles[index].b = blue;
			this->particles[index].velocity = velocity;
			this->particles[index].active = true;
			this->particles[index].scale = 0.4f;
			this->particles[index].uCoord = 0.25f;
			this->particles[index].time = 0.0f;

			this->currentParticleCnt++;
		}

	}

	return;
}

void EmitterPrototype::UpdateParticles(float dT)
{
	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	/*for (int i = 0; i<this->currentParticleCnt; i++)
	{
		this->particles[i].y = this->particles[i].y - (this->particles[i].velocity * dT / 1000);
	}*/
	float size = 1.0f / 3.0f;
	for (int i = 0; i<this->currentParticleCnt; i++)
	{
		this->particles[i].time += dT / (1000000);
		//this->particles[i].y = this->particles[i].y - (this->particles[i].velocity * dT / 1000);
		float x = 0, y = 0;
		float period = 8.0f, min = 0, max = 4.0f;
		float time = this->particles[i].time * this->particles[i].velocity;
		float width = 40;
		//Algorithm::GetSawtoothWave(x, y, this->particles[i].time, period, min, max);
		//Algorithm::GetEllipse(x, y, time, 8, 8);
		Algorithm::GetHypotrochoid(x, y, time, 6, 2, 10);
		//Algorithm::GetTriangleWave(x, y, time, period, min, max);
		this->particles[i].x = x * size;
		this->particles[i].z = y * size;
	}

	return;
}

void EmitterPrototype::KillParticles()
{
	//Go through all particles
	for (int i = 0; i < this->maxParticles; i++)
	{
		//The conditions for killing / restarting the particle
		if (this->particles[i].active && this->particles[i].y < -800000000.0f)
		{
			//moce the last used particle to this one
			this->particles[i] = this->particles[this->currentParticleCnt];
			//Kill the particle
			this->particles[this->currentParticleCnt].active = false;
			this->currentParticleCnt--;
			//for (int j = i; j < this->maxParticles - 1; j++)
			//{
			//	this->particles[j] = this->particles[j + 1];
			//	/*this->particles[j].x = this->particles[j + 1].x;
			//	this->particles[j].y = this->particles[j + 1].y;
			//	this->particles[j].z = this->particles[j + 1].z;
			//	this->particles[j].scale = this->particles[j + 1].scale;
			//	this->particles[j].r = this->particles[j + 1].r;
			//	this->particles[j].g = this->particles[j + 1].g;
			//	this->particles[j].b = this->particles[j + 1].b;
			//	this->particles[j].rotation = this->particles[j + 1].rotation;
			//	this->particles[j].active = this->particles[j + 1].active;
			//	this->particles[j].velocity = this->particles[j + 1].velocity;*/
			//}
		}
	}
}

bool EmitterPrototype::UpdateBuffers(ID3D11DeviceContext * deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Initialize vertex array to zeros at first.
	memset(this->vertices, 0, (sizeof(VertexType) * this->vertexCount));

	for (int i = 0; i < this->currentParticleCnt; i++)
	{
		this->vertices[i].position = DirectX::XMFLOAT4(this->particles[i].x, this->particles[i].y, this->particles[i].z, this->particles[i].scale);
		this->vertices[i].color = DirectX::XMFLOAT4(this->particles[i].r, this->particles[i].g, this->particles[i].b, this->particles[i].uCoord);

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

void EmitterPrototype::RenderBuffers(ID3D11DeviceContext * deviceContext)
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
