#include "LightShaderHandler.h"

LightShaderHandler::LightShaderHandler()
{
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->layout = nullptr;
	this->matrixBuffer = nullptr;
	this->samplerState = nullptr;
}

LightShaderHandler::~LightShaderHandler()
{
}

bool LightShaderHandler::Initialize(ID3D11Device* device, HWND* hwnd, int nrOfResources)
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

	WCHAR* vsFilename = L"../SmallGameProject/LightVertexShader.hlsl";
	WCHAR* psFilename = L"../SmallGameProject/LightPixelShader.hlsl";

	//Compile the vertex shader code
	hresult = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(hresult)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else {
			MessageBox(*hwnd, L"D3DCompileFromFile(VS)", L"Error", MB_OK);
		}
		return false;
	}

	//Compile the pixel shader code
	hresult = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(hresult)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else {
			MessageBox(*hwnd, L"D3DCompileFromFile(PS)", L"Error", MB_OK);
		}
		return false;
	}


	//Create the vertex shader from buffer
	hresult = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &this->vertexShader);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateVertexShader", L"Error", MB_OK);
		return false;
	}

	//Create the pixel shader from buffer
	hresult = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &this->pixelShader);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreatePixelShader", L"Error", MB_OK);
		return false;
	}



	//Fill the vertex input layout description 
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
	polygonLayout[1].AlignedByteOffset = 12;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get the number of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout.
	hresult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->layout);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateInputLayout", L"Error", MB_OK);
		return false;
	}

	//Release and nullptr the buffers as they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;


	//Fill the description of the dynamic matrix constant buffer that is in the vertex shader
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(LightMatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hresult = device->CreateBuffer(&matrixBufferDesc, NULL, &this->matrixBuffer);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateBufferM", L"Error", MB_OK);
		return false;
	}

	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(LightsCB);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hresult = device->CreateBuffer(&matrixBufferDesc, NULL, &this->lightBuffer);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateBufferL", L"Error", MB_OK);
		return false;
	}

	//Fill the texture sampler state description
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		MessageBox(*hwnd, L"device->CreateSamplerState", L"Error", MB_OK);
		return false;
	}

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hresult = device->CreateSamplerState(&samplerDesc, &this->shadowSamplerState);

	if (FAILED(hresult))
	{
		MessageBox(*hwnd, L"device->CreateSamplerState", L"Error", MB_OK);
		return false;
	}

	this->nrOfShaderResources = nrOfResources;

	this->nullResource = new ID3D11ShaderResourceView*[this->nrOfShaderResources];
	for (int i = 0; i < this->nrOfShaderResources; i++) {
		nullResource[i] = nullptr;
	}

	return true;
}

void LightShaderHandler::Shutdown()
{
	if (this->nullResource) {
		for (int i = 0; i < this->nrOfShaderResources; i++) {
			if (this->nullResource[i]) {
				this->nullResource[i]->Release();
				this->nullResource[i] = nullptr;
			}
		}
		delete[] this->nullResource;
	}
	
	//Release sampler state
	if (this->samplerState) {
		this->samplerState->Release();
		this->samplerState = nullptr;
	}
	//Release shadow sampler state
	if (this->shadowSamplerState) {
		this->shadowSamplerState->Release();
		this->shadowSamplerState = nullptr;
	}
	//Release matrix constant buffer
	if (this->matrixBuffer) {
		this->matrixBuffer->Release();
		this->matrixBuffer = nullptr;
	}
	//Release light constant buffer
	if (this->lightBuffer) {
		this->lightBuffer->Release();
		this->lightBuffer = nullptr;
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

void LightShaderHandler::ResetPSShaderResources(ID3D11DeviceContext * deviceContext)
{	
	deviceContext->PSSetShaderResources(0, this->nrOfShaderResources, this->nullResource);
}


bool LightShaderHandler::Render(ID3D11DeviceContext* deviceContext, int indexCount, LightShaderParameters* params)
{
	bool result = false;

	//Set shader parameters used for rendering
	result = this->SetShaderParameters(deviceContext, params);
	if (!result) {
		return false;
	}

	this->RenderShader(deviceContext, indexCount);

	return true;
}

void LightShaderHandler::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND* hwnd, WCHAR* shaderFilename)
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

	//Notify the user to check error log
	MessageBox(*hwnd, L"Error compiling shader, check shader_error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool LightShaderHandler::SetShaderParameters(ID3D11DeviceContext* deviceContext, LightShaderParameters* params)
{
	HRESULT hresult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightMatrixBuffer* dataPtrM;
	LightsCB* dataPtrL;

	//Transpose each matrix to prepare for shaders (requirement in directx 11)
	/*params->worldMatrix = DirectX::XMMatrixTranspose(params->worldMatrix);
	params->viewMatrix = DirectX::XMMatrixTranspose(params->viewMatrix);
	params->projectionMatrix = DirectX::XMMatrixTranspose(params->projectionMatrix);
	params->lightViewMatrix = DirectX::XMMatrixTranspose(params->lightViewMatrix);
	params->lightProjectionMatrix = DirectX::XMMatrixTranspose(params->lightProjectionMatrix);*/

	ZeroMemory(&mappedResource, sizeof(mappedResource));
	//Map the constant buffer so we can write to it (denies GPU access)
	hresult = deviceContext->Map(this->matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		return false;
	}

	//Get pointer to the data
	dataPtrM = (LightMatrixBuffer*)mappedResource.pData;

	//Copy the matrices to the constant buffer

	dataPtrM->view = params->viewMatrix;
	dataPtrM->projection = params->projectionMatrix;
	dataPtrM->lightView = params->lightViewMatrix;
	dataPtrM->lightProjection = params->lightProjectionMatrix;

	dataPtrM->camPos = params->camPos;

	//Unmap the constant buffer to give the GPU access agin
	deviceContext->Unmap(this->matrixBuffer, 0);

	ZeroMemory(&mappedResource, sizeof(mappedResource));
	//Map the constant buffer so we can write to it (denies GPU access)
	hresult = deviceContext->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		return false;
	}

	//Get pointer to the data
	dataPtrL = (LightsCB*)mappedResource.pData;

	dataPtrL->Diffuse[0] = params->dirLight.Diffuse;
	dataPtrL->Ambient[0] = params->dirLight.Ambient;
	dataPtrL->Specular[0] = params->dirLight.Specular;
	dataPtrL->Position[0] = params->dirLight.Direction;

	dataPtrL->activeLights = params->pointLights.size();
	for (int i = 1; i <= dataPtrL->activeLights; i++) {
		PointLight currLight = params->pointLights.at(i-1);
		dataPtrL->Diffuse[i] = currLight.Diffuse;
		dataPtrL->Ambient[i] = currLight.Ambient;
		dataPtrL->Specular[i] = currLight.Specular;
		dataPtrL->Position[i] = currLight.Position;
		dataPtrL->Attenuation[i-1] = currLight.Attenuation;
	}

	//Unmap the constant buffer to give the GPU access agin
	deviceContext->Unmap(this->lightBuffer, 0);

	//Set constant buffer position in vertex shader
	ID3D11Buffer* pixelConstants[2];
	pixelConstants[0] = this->matrixBuffer;
	pixelConstants[1] = this->lightBuffer;
	//Set the constant buffer in vertex and pixel shader with updated values
	deviceContext->VSSetConstantBuffers(0, 1, &this->matrixBuffer);
	deviceContext->PSSetConstantBuffers(0, 2, pixelConstants);
	/*deviceContext->PSSetConstantBuffers(bufferNumber, 2, &this->matrixBuffer);
	bufferNumber = 1;
	deviceContext->PSSetConstantBuffers(1, 2, &this->lightBuffer);*/

	if (params->deferredTextures) {
		//Set shader texture resource for pixel shader
		deviceContext->PSSetShaderResources(0, 5, params->deferredTextures);
	}

	if (params->shadowTexture)
	{
		deviceContext->PSSetShaderResources(5, 1, &params->shadowTexture);
	}

	return true;
}

void LightShaderHandler::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set the input layout for vertex
	deviceContext->IASetInputLayout(this->layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(this->vertexShader, NULL, 0);
	deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->PSSetShader(this->pixelShader, NULL, 0);
	//Set the sampler state in pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->samplerState);
	deviceContext->PSSetSamplers(1, 1, &this->shadowSamplerState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}