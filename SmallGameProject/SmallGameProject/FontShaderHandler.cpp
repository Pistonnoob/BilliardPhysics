#include "FontShaderHandler.h"

FontShaderHandler::FontShaderHandler()
{
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->layout = nullptr;
	this->matrixBuffer = nullptr;
	this->samplerState = nullptr;
}

FontShaderHandler::~FontShaderHandler()
{
}

bool FontShaderHandler::Initialize(ID3D11Device* device)
{
	HRESULT hresult;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//init pointers to nullptr
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	WCHAR* vsFilename = L"../SmallGameProject/FontVertexShader.hlsl";
	WCHAR* psFilename = L"../SmallGameProject/FontPixelShader.hlsl";

	//Compile the vertex shader code
	hresult = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(hresult)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, vsFilename);
		}
		return false;
	}

	//Compile the pixel shader code
	hresult = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(hresult)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		return false;
	}

	//Create the vertex shader from buffer
	hresult = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &this->vertexShader);
	if (FAILED(hresult)) {
		return false;
	}

	//Create the pixel shader from buffer
	hresult = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->pixelShader);
	if (FAILED(hresult)) {
		return false;
	}

	//Fill the vertex input layout description 
	//This needs to match the Model and shader 
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get the number of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout.
	hresult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->layout);
	if (FAILED(hresult)) {
		return false;
	}

	//Release and nullptr the buffers as they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	//Fill the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(FontConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hresult = device->CreateBuffer(&matrixBufferDesc, NULL, &this->matrixBuffer);
	if (FAILED(hresult)) {
		return false;
	}

	//Fill the texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the texture sampler state
	hresult = device->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(hresult))
	{
		return false;
	}

	

	return true;
}

void FontShaderHandler::Shutdown()
{
	//Release sampler state
	if (this->samplerState) {
		this->samplerState->Release();
		this->samplerState = nullptr;
	}
	//Release matrix constant buffer
	if (this->matrixBuffer) {
		this->matrixBuffer->Release();
		this->matrixBuffer = nullptr;
	}
	//Release layout
	if (this->layout) {
		this->layout->Release();
		this->layout = nullptr;
	}

	//Release pixel shader
	if (this->pixelShader) {
		this->pixelShader->Release();
		this->pixelShader = nullptr;
	}

	//Release vertex shader
	if (this->vertexShader) {
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	return;
}


bool FontShaderHandler::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix,
	DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* fontTexture, DirectX::XMFLOAT3 color)
{
	bool result = false;

	//Set shader parameters used for rendering
	result = this->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, fontTexture, color);
	if (!result) {
		return false;
	}

	this->RenderShader(deviceContext, indexCount);

	return true;
}

void FontShaderHandler::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;

	//Get a pointer to the error message text buffer
	compileErrors = (char*)errorMessage->GetBufferPointer();

	//Get the length of the message
	bufferSize = errorMessage->GetBufferSize();

	//Open a file to write error messages to
	fout.open("shader_error.txt");

	//Write the error message to the file
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//Close the file
	fout.close();

	//Release the error message
	errorMessage->Release();
	errorMessage = nullptr;

	return;
}

bool FontShaderHandler::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix,
	DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* fontTexture, DirectX::XMFLOAT3 color)
{
	HRESULT hresult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	FontConstantBuffer* dataPtr;
	unsigned int bufferNumber;

	//Transpose each matrix to prepare for shaders (requirement in directx 11)
	/*worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);*/

	//Map the constant buffer so we can write to it (denies GPU access)
	hresult = deviceContext->Map(this->matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		return false;
	}

	//Get pointer to the data
	dataPtr = (FontConstantBuffer*)mappedResource.pData;

	//Copy the matrices to the constant buffer
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->color = color;

	//Unmap the constant buffer to give the GPU access agin
	deviceContext->Unmap(this->matrixBuffer, 0);

	//Set constant buffer position in vertex shader
	bufferNumber = 0;

	//Set the constant buffer in vertex and pixel shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer);

	if (fontTexture) {
		//Set shader color texture resource for pixel shader
		deviceContext->PSSetShaderResources(0, 1, &fontTexture);
	}

	return true;
}

void FontShaderHandler::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the input layout for vertex
	deviceContext->IASetInputLayout(this->layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(this->vertexShader, NULL, 0);
	deviceContext->PSSetShader(this->pixelShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);

	//Set the sampler state in pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->samplerState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}