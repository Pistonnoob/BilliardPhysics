#include "ScreenQuad.h"

ScreenQuad::ScreenQuad()
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
}

ScreenQuad::~ScreenQuad()
{
}

bool ScreenQuad::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight)
{
	VertexSimple vertices[6];
	unsigned long indices[6];
	int sizeVertices = 6;
	int sizeIndices = 6;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	float left, right, top, bottom;
	HRESULT hresult;
	bool result;

	//Calculate the screen coordinates of the left side of the window
	left = (float)((screenWidth / 2) * -1);

	//Calculate the screen coordinates of the right side of the window
	right = left + (float)screenWidth;

	//Calculate the screen coordinates of the top of the window
	top = (float)(screenHeight / 2);

	//Calculate the screen coordinates of the bottom of the window
	bottom = top - (float)screenHeight;

	//Load the vertex array with data
	//First triangle
	vertices[0].position = DirectX::XMFLOAT3(left, top, 0.0f);  //Top left
	vertices[0].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  //Bottom right
	vertices[1].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = DirectX::XMFLOAT3(left, bottom, 0.0f);  //Bottom left
	vertices[2].texture = DirectX::XMFLOAT2(0.0f, 1.0f);

	//Second triangle
	vertices[3].position = DirectX::XMFLOAT3(left, top, 0.0f);  //Top left
	vertices[3].texture = DirectX::XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = DirectX::XMFLOAT3(right, top, 0.0f);  //Top right
	vertices[4].texture = DirectX::XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = DirectX::XMFLOAT3(right, bottom, 0.0f);  //Bottom right
	vertices[5].texture = DirectX::XMFLOAT2(1.0f, 1.0f);

	//Load the index array with data
	for (int i = 0; i < sizeIndices; i++)
	{
		indices[i] = i;
	}

	//Set the description of the static vertex buffer
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexSimple) * sizeVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sizeIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &this->indexBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	return true;
}

void ScreenQuad::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(VertexSimple);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ScreenQuad::Shutdown()
{
	//Release the index buffer
	if (this->indexBuffer) {
		this->indexBuffer->Release();
		this->indexBuffer = nullptr;
	}

	//Release the vertex buffer
	if (this->vertexBuffer) {
		this->vertexBuffer->Release();
		this->vertexBuffer = nullptr;
	}

	return;
}