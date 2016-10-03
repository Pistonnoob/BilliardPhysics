#include "DeferredShaderHandler.h"

DeferredShaderHandler::DeferredShaderHandler()
{
	this->vertexShader = nullptr;
	this->geoShader = nullptr;
	this->pixelShader = nullptr;
	this->layout = nullptr;
	this->matrixBuffer = nullptr;
	this->samplerState = nullptr;

	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->deferredRenderTargetTextures[i] = nullptr;
		this->deferredRenderTargetViews[i] = nullptr;
		this->deferredShaderResources[i] = nullptr;
	}
}

DeferredShaderHandler::~DeferredShaderHandler()
{
}

bool DeferredShaderHandler::Initialize(ID3D11Device* device, HWND* hwnd, int screenWidth, int screenHeight)
{
	HRESULT hresult;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* geoShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	//init pointers to nullptr
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;
	geoShaderBuffer = nullptr;

	WCHAR* vsFilename = L"../SmallGameProject/DeferredVertexShader.hlsl";
	WCHAR* gsFilename = L"../SmallGameProject/DeferredGeometryShader.hlsl";
	WCHAR* psFilename = L"../SmallGameProject/DeferredPixelShader.hlsl";

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

	//Compile the geometry shader code
	hresult = D3DCompileFromFile(gsFilename, NULL, NULL, "main", "gs_5_0", D3DCOMPILE_DEBUG, 0, &geoShaderBuffer, &errorMessage);
	if (FAILED(hresult)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		}
		else {
			MessageBox(*hwnd, L"D3DCompileFromFile(Geo)", L"Error", MB_OK);
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

	//Create the geometry shader from buffer
	hresult = device->CreateGeometryShader(geoShaderBuffer->GetBufferPointer(), geoShaderBuffer->GetBufferSize(), NULL, &this->geoShader);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateGeometryShader", L"Error", MB_OK);
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
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//Get the number of elements in the layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//Create the vertex input layout.
	hresult = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->layout);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateInputLayout Deferred", L"Error", MB_OK);
		return false;
	}

	//Release and nullptr the buffers as they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	geoShaderBuffer->Release();
	geoShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;
	

	//Fill the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(CBPerObj);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	hresult = device->CreateBuffer(&matrixBufferDesc, NULL, &this->matrixBuffer);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"device->CreateBuffer", L"Error", MB_OK);
		return false;
	}

	//Fill the texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
		MessageBox(*hwnd, L"device->CreateSamplerState", L"Error", MB_OK);
		return false;
	}

	D3D11_TEXTURE2D_DESC renderTextureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	//Initialize the render target texture description
	ZeroMemory(&renderTextureDesc, sizeof(renderTextureDesc));

	//Setup the render target texture description
	renderTextureDesc.Width = screenWidth;
	renderTextureDesc.Height = screenHeight;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;

	//Create the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hresult = device->CreateTexture2D(&renderTextureDesc, NULL, &this->deferredRenderTargetTextures[i]);
		if (FAILED(hresult)) {
			MessageBox(*hwnd, L"device->CreateTexture2D", L"Error", MB_OK);
			return false;
		}
	}

	//Setup the description of the render target views
	renderTargetViewDesc.Format = renderTextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	//Create the render target views
	for (int i = 0; i<BUFFER_COUNT; i++) {
		hresult = device->CreateRenderTargetView(this->deferredRenderTargetTextures[i], &renderTargetViewDesc, &this->deferredRenderTargetViews[i]);
		if (FAILED(hresult)) {
			MessageBox(*hwnd, L"device->CreateRenderTargetView", L"Error", MB_OK);
			return false;
		}
	}

	//Setup the description of the shader resource view
	shaderResourceViewDesc.Format = renderTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	//Create the shader resource views
	for (int i = 0; i < BUFFER_COUNT; i++) {
		hresult = device->CreateShaderResourceView(this->deferredRenderTargetTextures[i], &shaderResourceViewDesc, &this->deferredShaderResources[i]);
		if (FAILED(hresult)) {
			MessageBox(*hwnd, L"device->CreateShaderResourceView", L"Error", MB_OK);
			return false;
		}
	}
	
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	//Depth buffer desc
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//Fill depth buffer description	
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create depth texture
	hresult = device->CreateTexture2D(&depthBufferDesc, NULL, &this->depthStencilBuffer);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"this->device->CreateTexture2D", L"Error", MB_OK);
		return false;
	}

	//Init depth stencil view description
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Fill the desc
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//Create the depth stencil view
	hresult = device->CreateDepthStencilView(this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView);
	if (FAILED(hresult)) {
		MessageBox(*hwnd, L"this->device->CreateDepthStencilView", L"Error", MB_OK);
		return false;
	}

	return true;
}

void DeferredShaderHandler::Shutdown()
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

	//Release the deferred render targets
	for (int i = 0; i < BUFFER_COUNT; i++) {
		if (this->deferredRenderTargetTextures[i]) {
			this->deferredRenderTargetTextures[i]->Release();
			this->deferredRenderTargetTextures[i] = nullptr;
		}
		if (this->deferredRenderTargetViews[i]) {
			this->deferredRenderTargetViews[i]->Release();
			this->deferredRenderTargetViews[i] = nullptr;
		}
		if (this->deferredShaderResources[i]) {
			this->deferredShaderResources[i]->Release();
			this->deferredShaderResources[i] = nullptr;
		}
	}

	//Release the depth stuff
	if (this->depthStencilBuffer) {
		this->depthStencilBuffer->Release();
		this->depthStencilBuffer = nullptr;
	}
	if (this->depthStencilView) {
		this->depthStencilView->Release();
		this->depthStencilView = nullptr;
	}

	//Release pixel shader
	if (this->pixelShader) {
		this->pixelShader->Release();
		this->pixelShader = nullptr;
	}

	//Release geometry shader
	if (this->geoShader) {
		this->geoShader->Release();
		this->geoShader = nullptr;
	}

	//Release vertex shader
	if (this->vertexShader) {
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	return;
}

bool DeferredShaderHandler::Render(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart, DeferredShaderParameters* params)
{
	bool result = false;

	//Set shader parameters used for rendering
	result = this->SetShaderParameters(deviceContext, params);
	if (!result) {
		return false;
	}

	this->RenderShader(deviceContext, indexCount, indexStart);

	return true;
}

void DeferredShaderHandler::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND* hwnd, WCHAR* shaderFilename)
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

bool DeferredShaderHandler::SetShaderParameters(ID3D11DeviceContext* deviceContext, DeferredShaderParameters* params)
{
	HRESULT hresult;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CBPerObj* dataPtr;
	unsigned int bufferNumber;

	//Transpose each matrix to prepare for shaders (requirement in directx 11)
	/*params->worldMatrix = XMMatrixTranspose(params->worldMatrix);
	params->viewMatrix = XMMatrixTranspose(params->viewMatrix);
	params->projectionMatrix = XMMatrixTranspose(params->projectionMatrix);*/

	//Map the constant buffer so we can write to it (denies GPU access)
	hresult = deviceContext->Map(this->matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hresult)) {
		return false;
	}

	//Get pointer to the data
	dataPtr = (CBPerObj*)mappedResource.pData;

	//Copy the matrices to the constant buffer
	dataPtr->world = params->worldMatrix;
	dataPtr->view = params->viewMatrix;
	dataPtr->projection = params->projectionMatrix;

	dataPtr->diffColor = params->diffColor;
	dataPtr->ambientColor = params->ambientColor;
	dataPtr->specColor = params->specColor;

	dataPtr->camPos = params->camPos;

	if (!params->diffTexture) {
		dataPtr->hasTexture = false;
	}
	else {
		dataPtr->hasTexture = true;
	}

	//Unmap the constant buffer to give the GPU access agin
	deviceContext->Unmap(this->matrixBuffer, 0);

	//Set constant buffer position in vertex shader
	bufferNumber = 0;

	//Set the constant buffer in vertex and pixel shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer);
	deviceContext->GSSetConstantBuffers(0, 0, nullptr);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer);

	if (params->diffTexture) {
		//Set shader texture resource for pixel shader
		deviceContext->PSSetShaderResources(0, 1, &params->diffTexture);
	}
	return true;
}

void DeferredShaderHandler::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart)
{
	//Set the input layout for vertex
	deviceContext->IASetInputLayout(this->layout);

	//Set the vertex and pixel shaders that will be used to render this triangle
	deviceContext->VSSetShader(this->vertexShader, NULL, 0);
	deviceContext->PSSetShader(this->pixelShader, NULL, 0);
	deviceContext->GSSetShader(this->geoShader, NULL, 0);
	//Set the sampler state in pixel shader
	deviceContext->PSSetSamplers(0, 1, &this->samplerState);

	//Render the triangle
	deviceContext->DrawIndexed(indexCount, indexStart, 0);

	return;
}

void DeferredShaderHandler::SetDeferredRenderTargets(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, this->deferredRenderTargetViews, this->depthStencilView);
}

void DeferredShaderHandler::ClearRenderTargets(ID3D11DeviceContext* deviceContext)
{
	float color[4];

	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	//Clear the render target textures
	for (int i = 0; i < BUFFER_COUNT; i++) {
		deviceContext->ClearRenderTargetView(this->deferredRenderTargetViews[i], color);
	}

	//Clear the depth buffer
	deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

int DeferredShaderHandler::GetBufferCount()
{
	return BUFFER_COUNT;
}

ID3D11ShaderResourceView** DeferredShaderHandler::GetShaderResourceViews()
{
	return this->deferredShaderResources;
}

ID3D11DepthStencilView * DeferredShaderHandler::GetDepthView()
{
	return this->depthStencilView;
}
