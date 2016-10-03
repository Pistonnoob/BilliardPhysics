#include "ParticleEmitter.h"



ParticleEmitter::ParticleEmitter()
{
	this->accumulatedTime = 0.0f;
	this->emitterTime = 1.0f;
	this->cameraPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->currentParticleCnt = 0;
	this->maxParticles = 0;
	this->texture = nullptr;
	this->world = DirectX::XMMatrixIdentity();
	this->isCompleted = false;


	this->indexCount = this->vertexCount = 0;
	this->indexBuffer = this->vertexBuffer = nullptr;
}


ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::Shutdown()
{
	if (this->vertexBuffer)
	{
		this->vertexBuffer->Release();
		this->vertexBuffer = nullptr;
	}

	if (this->indexBuffer)
	{
		this->indexBuffer->Release();
		this->indexBuffer = nullptr;
	}

	if (this->texture)
		this->texture = nullptr;

	if (this->vertices)
	{
		delete[] this->vertices;
		this->vertices = nullptr;
	}

	this->ShutdownSpecific();
}

bool ParticleEmitter::Initialize(ID3D11Device * device, ID3D11ShaderResourceView * texture, float timeLimit)
{
	bool result = false;
	//Set the texture
	this->texture = texture;
	//Set the time limit for emitting particles
	this->emitterTime = timeLimit;

	this->world = DirectX::XMMatrixIdentity();
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->vertexCount = this->indexCount = 0;
	this->currentParticleCnt = 0;
	this->accumulatedTime = 0.0f;
	this->cameraPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);



	//Initialize the buffers
	result = this->InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void ParticleEmitter::SetCameraPos(DirectX::XMFLOAT3 cameraPos)
{
	this->cameraPos = cameraPos;
}

void ParticleEmitter::SetCameraPos(DirectX::XMFLOAT4 cameraPos)
{
	this->cameraPos = DirectX::XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z);
}

void ParticleEmitter::SetWorld(DirectX::XMMATRIX world)
{
	this->world = world;
}

void ParticleEmitter::ApplyPosition(DirectX::XMFLOAT3 deltaPosition)
{
	this->ApplyPosition(deltaPosition.x, deltaPosition.y, deltaPosition.z);
}

void ParticleEmitter::ApplyPosition(float dX, float dY, float dZ)
{
	this->world *= DirectX::XMMatrixTranslation(dX, dY, dZ);
}

void ParticleEmitter::ApplyMatrix(DirectX::XMMATRIX toApply)
{
	this->world *= toApply;
}

void ParticleEmitter::GetWorld(DirectX::XMMATRIX& storeIn)
{
	storeIn = this->world;
}

ID3D11ShaderResourceView * ParticleEmitter::GetTexture()
{
	return this->texture;
}



bool ParticleEmitter::Update(float dT, ID3D11DeviceContext * deviceContext)
{
	// Increment the frame time.
	this->accumulatedTime += dT;
	this->emitterTime -= dT / 1000000;
	return this->UpdateSpecific(dT, deviceContext);
}

bool ParticleEmitter::distanceToCamera(float x, float y, float z)
{
	return false;
}

bool ParticleEmitter::IsCompleted()
{
	return this->isCompleted || this->emitterTime <= 0;
}

bool ParticleEmitter::InitializeBuffers(ID3D11Device * device)
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

void ParticleEmitter::RenderBuffers(ID3D11DeviceContext * deviceContext)
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


