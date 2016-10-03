#include "TextHandler.h"


TextHandler::TextHandler()
{
	this->baseViewMatrix = DirectX::XMMatrixIdentity();
	this->font = nullptr;
}

TextHandler::TextHandler(const TextHandler& originalObj)
{

}

TextHandler::~TextHandler()
{

}

int TextHandler::CreateSentence(ID3D11Device* device, int maxLength)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT hresult;
	bool result;
	Sentence* tempSentence = new Sentence;

	tempSentence->maxLength = maxLength; //Init the max character length of the sentence
	tempSentence->vertexCount = tempSentence->indexCount = maxLength * 6; //Each letter has 6 vertices

	Vertex* vertices = new Vertex[tempSentence->vertexCount]; //Create an empty vertex array and init to 0
	memset(vertices, 0, (sizeof(Vertex) * tempSentence->vertexCount));

	unsigned long* indices = new unsigned long[tempSentence->indexCount]; //Create and fill the index buffer
	for (int i = 0; i < tempSentence->indexCount; i++) {
		indices[i] = i;
	}

	//Set the description of the static vertex buffer
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * tempSentence->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &tempSentence->vertexBuffer);
	if (FAILED(hresult)) {
		return -1;
	}

	//Set up the description of the static index buffer
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * tempSentence->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &tempSentence->indexBuffer);
	if (FAILED(hresult)) {
		return -1;
	}

	this->sentences.push_back(tempSentence); //Store the sentence obejct

	delete[] vertices; //Delete no longer needed arrays
	delete[] indices;

	return this->sentences.size() - 1; //Return string id, starts from 0
}

bool TextHandler::UpdateSentence(ID3D11DeviceContext* deviceContext, int id, const std::string& text, int posX, int posY, const DirectX::XMFLOAT3& color, float size)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Sentence* tempSentence = this->sentences.at(id); //Get the sentence by id
	if (!tempSentence) {
		return false;
	}

	int nrLetters = text.size();
	if (nrLetters > tempSentence->maxLength) { //Check so the new text isn't longer then it can be
		return false;
	}

	Vertex* vertices = new Vertex[tempSentence->vertexCount]; //Create an empty vertex array and init to 0
	memset(vertices, 0, (sizeof(Vertex) * tempSentence->vertexCount));

	float drawPosX = ((this->screenWidth / 2) * -1) + posX; //Calculate position in window to draw on
	float drawPosY = (this->screenHeight / 2) - posY;

	this->font->BuildVertexArray((void*)vertices, text.c_str(), drawPosX, drawPosY, size); //Ask the font object to build the vertex array

	result = deviceContext->Map(tempSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); //Map the vertex buffer and discard it's content
	if (FAILED(result))
	{
		return false;
	}

	Vertex* verticesPtr = (Vertex*)mappedResource.pData;

	//Copy the new vertex data to buffer
	memcpy(verticesPtr, vertices, (sizeof(Vertex) * tempSentence->vertexCount));

	deviceContext->Unmap(tempSentence->vertexBuffer, 0);

	delete[] vertices;

	tempSentence->color = color;

	return true;
}

void TextHandler::ReleaseSentences()
{
	for (int i = 0; i < this->sentences.size(); i++) {
		if (this->sentences.at(i)) {
			this->sentences.at(i)->ShutdownBuffers();
			delete this->sentences.at(i);
			this->sentences.at(i) = nullptr;
		}
	}

	return;
}

void TextHandler::RenderSentence(ID3D11DeviceContext* deviceContext, int id, int& indexCount, ID3D11ShaderResourceView*& texture, DirectX::XMFLOAT3& color)
{
	Sentence* tempSentence = this->sentences.at(id);

	indexCount = tempSentence->indexCount;
	texture = this->font->GetTexture();
	color = tempSentence->color;

	unsigned int stride;
	unsigned offset;

	//Set vertex buffer stride and offset
	stride = sizeof(Vertex);
	offset = 0;

	//Set the vertex buffer to active in the input assembly so it can rendered
	deviceContext->IASetVertexBuffers(0, 1, &tempSentence->vertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer(tempSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type od primitiv that should be rendered from this vertex buffer, in this case triangles
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

int TextHandler::GetNrOfSentences()
{
	return this->sentences.size();
}

bool TextHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX baseViewMatrix, int screenWidth, int screenHeight)
{
	bool result;

	this->font = new Font;
	if (!this->font) {
		return false;
	}
	result = this->font->Initialize(device, deviceContext, "fontData.txt", "font.mtl");
	if (!result) {
		return false;
	}

	

	this->baseViewMatrix = baseViewMatrix;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	return true;
}

void TextHandler::Shutdown()
{
	this->ReleaseSentences();

	if (this->font) {
		this->font->Shutdown();
		delete this->font;
		this->font = nullptr;
	}

	return;
}