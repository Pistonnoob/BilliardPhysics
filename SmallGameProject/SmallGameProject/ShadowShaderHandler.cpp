#include "ShadowShaderHandler.h"

ShadowShaderHandler::ShadowShaderHandler()
{
	this->StartUp();
}

ShadowShaderHandler::~ShadowShaderHandler()
{
	this->Shutdown();
}

bool ShadowShaderHandler::Initialize(ID3D11Device * gDevice, HWND * hwnd, int nrOfResources, int screenWidth, int screenHeight) throw(...)
{
	this->viewPort = new D3D11_VIEWPORT;

	float width = 1024.0f;
	float height = 1024.0f;

	this->viewPort->TopLeftX = 0.0f;
	this->viewPort->TopLeftY = 0.0f;
	this->viewPort->Width = width;
	this->viewPort->Height = height;
	this->viewPort->MinDepth = 0.0f;
	this->viewPort->MaxDepth = 1.0f;

	try
	{
		this->Create2DTexture(gDevice, screenWidth, screenHeight);

		this->CreateDepthStencilView(gDevice);
		this->CreateShaderResourceView(gDevice);

		this->LoadVertexShaderFromFile();
		this->CreateVertexLayout(gDevice);
		this->CreateVertexShader(gDevice);

		this->CreateConstantBuffer(gDevice);
	}
	catch (char* e)
	{
		//hover at e to see error
		system("pause");
	}
	return true;
}

void ShadowShaderHandler::Shutdown()
{
	if (this->depthMap != nullptr)
	{
		this->depthMap->Release();
		this->depthMap = nullptr;
	}

	if (this->mDepthMapDSV != nullptr)
	{
		this->mDepthMapDSV->Release();
		this->mDepthMapDSV = nullptr;
	}

	if (this->mDepthMapSRV != nullptr)
	{
		this->mDepthMapSRV->Release();
		this->mDepthMapSRV = nullptr;
	}

	if (this->matrixBuffer != nullptr)
	{
		this->matrixBuffer->Release();
		this->matrixBuffer = nullptr;
	}

	if (this->vertexShader != nullptr)
	{
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	if (this->layout != nullptr)
	{
		this->layout->Release();
		this->layout = nullptr;
	}

	if (this->viewPort != nullptr)
	{
		delete this->viewPort;
		this->viewPort = nullptr;
	}

	if (this->vertexShaderBuffer != nullptr)
	{
		this->vertexShaderBuffer->Release();
		this->vertexShaderBuffer = nullptr;
	}
	
	if (this->vertexShaderBuffer != nullptr)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;
	}
}

void ShadowShaderHandler::SetRenderTarget(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->RSSetViewports(1, this->viewPort);
	gDeviceContext->OMSetRenderTargets(0, nullptr, this->mDepthMapDSV);
}

bool ShadowShaderHandler::Render(ID3D11DeviceContext * deviceContext, int indexCount, int indexStart, ShadowShaderParameters * params)
{
	bool result = false;

	//Set shader parameters used for rendering
	try
	{
		this->SetShaderParameters(deviceContext, params);
		result = true;
	}
	catch (char* e)
	{
		system("pause");
	}
	
	if (result == false) {
		return false;
	}

	this->RenderShader(deviceContext, indexCount, indexStart);
	return true;
}

void ShadowShaderHandler::ClearShadowMap(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->ClearDepthStencilView(this->mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* ShadowShaderHandler::getShadowMapSRW() const
{
	return this->mDepthMapSRV;
}

void ShadowShaderHandler::StartUp()
{
	this->vertexShader			= nullptr;
	this->vertexShaderBuffer	= nullptr;
	this->layout				= nullptr;
	this->matrixBuffer			= nullptr;
	this->viewPort				= nullptr;
	this->mDepthMapSRV			= nullptr;
	this->mDepthMapDSV			= nullptr;
	this->depthMap				= nullptr;
}

void ShadowShaderHandler::SetViewPort(ID3D11DeviceContext * gDeviceContext)
{
	gDeviceContext->RSSetViewports(1,this->viewPort);
}

void ShadowShaderHandler::Create2DTexture(ID3D11Device * gDevice, int screenWidth, int screenHeight) throw(...)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	HRESULT resultHelper;

	UINT width = static_cast<UINT>(screenWidth);
	UINT height = static_cast<UINT>(screenHeight);

	texDesc.Width = 1024.0f;
	texDesc.Height = 1024.0f;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	resultHelper = gDevice->CreateTexture2D(&texDesc, 0, &this->depthMap);

	if (FAILED(resultHelper))
	{
		throw("failed to create depthMap 2Dtexture, in ShadowShaderHandler.cpp");
	}
}

void ShadowShaderHandler::CreateDepthStencilView(ID3D11Device * gDevice) throw(...)
{
	HRESULT resultHelper;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	resultHelper = gDevice->CreateDepthStencilView(this->depthMap, &dsvDesc, &this->mDepthMapDSV);
	
	if (FAILED(resultHelper))
	{
		throw("Failed creating the DepthStencilView");
	}
}

void ShadowShaderHandler::CreateShaderResourceView(ID3D11Device * gDevice) throw(...)
{
	HRESULT resultHelper;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	resultHelper = gDevice->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV);

	if (FAILED(resultHelper))
	{
		throw("Failed creating the shaderResourceView");
	}
}

void ShadowShaderHandler::LoadVertexShaderFromFile() throw(...)
{
	HRESULT resultHelper;
	
	std::string errorMsg;
	WCHAR* vsFilename = L"../SmallGameProject/ShadowMapVertexShader.hlsl";
	ID3D10Blob* errorMessage;


	resultHelper = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG, 0, &this->vertexShaderBuffer, &errorMessage);
	
	if (FAILED(resultHelper))
	{
		if (errorMessage) 
		{
			throw("read errorMessage");
		}
		else 
		{
			throw("compile from file error");
		}
	}
}

void ShadowShaderHandler::CreateVertexLayout(ID3D11Device * gDevice) throw(...)
{
	HRESULT resultHelper;
	int numElements = 0;

	D3D11_INPUT_ELEMENT_DESC shadowVertexLayout[1];

	shadowVertexLayout[0].SemanticName = "POSITIONL";
	shadowVertexLayout[0].SemanticIndex = 0;
	shadowVertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	shadowVertexLayout[0].InputSlot = 0;
	shadowVertexLayout[0].AlignedByteOffset = 0;
	shadowVertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	shadowVertexLayout[0].InstanceDataStepRate = 0;

	//Get the number of elements in the layout
	numElements = sizeof(shadowVertexLayout) / sizeof(shadowVertexLayout[0]);

	//Create the vertex input layout.
	resultHelper = gDevice->CreateInputLayout(shadowVertexLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &this->layout);
}

void ShadowShaderHandler::CreateVertexShader(ID3D11Device * gDevice) throw(...)
{
	HRESULT resultHelper;

	resultHelper = gDevice->CreateVertexShader(this->vertexShaderBuffer->GetBufferPointer(), this->vertexShaderBuffer->GetBufferSize(), NULL, &this->vertexShader);

	if (FAILED(resultHelper))
	{
		throw("Failed to create the vertex shader");
	}
}

void ShadowShaderHandler::CreateConstantBuffer(ID3D11Device * gDevice) throw(...)
{
	HRESULT resultHelper;

	D3D11_BUFFER_DESC matrixBufferDesc;
	//Fill the description of the dynamic matrix constant buffer that is in the vertex shader
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ShadowConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	resultHelper = gDevice->CreateBuffer(&matrixBufferDesc, NULL, &this->matrixBuffer);
	if (FAILED(resultHelper)) 
	{
		throw("Failed creating the buffer");
	}
}

bool ShadowShaderHandler::SetShaderParameters(ID3D11DeviceContext * deviceContext, ShadowShaderParameters * params) throw(...)
{
	HRESULT resultHelper;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ShadowConstantBuffer* dataPtr;
	unsigned int bufferNumber;

	//Transpose each matrix to prepare for shaders (requirement in directx 11)
	/*params->worldMatrix = XMMatrixTranspose(params->worldMatrix);
	params->viewMatrix = XMMatrixTranspose(params->viewMatrix);
	params->projectionMatrix = XMMatrixTranspose(params->projectionMatrix);*/

	//Map the constant buffer so we can write to it (denies GPU access)
	resultHelper = deviceContext->Map(this->matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(resultHelper)) 
	{
		throw("failed to map constant buffer");
	}

	//Get pointer to the data
	dataPtr = (ShadowConstantBuffer*)mappedResource.pData;

	//Copy the matrices to the constant buffer
	dataPtr->world = params->worldMatrix;
	dataPtr->view = params->viewMatrix;
	dataPtr->projection = params->projectionMatrix;

	//Unmap the constant buffer to give the GPU access agin
	deviceContext->Unmap(this->matrixBuffer, 0);

	//Set constant buffer position in vertex shader
	bufferNumber = 0;

	//Set the constant buffer in vertex and pixel shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer);
}

void ShadowShaderHandler::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount, int indexStart)
{
	//set input layout to the pipeline
	deviceContext->IASetInputLayout(this->layout);

	//set shaders
	deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->PSSetShader(nullptr, nullptr, 0);

	//draw map
	deviceContext->DrawIndexed(indexCount, indexStart, 0);
}

LPCWSTR ShadowShaderHandler::stringToLPCSTR(std::string toConvert) const
{
	std::wstring stemp = std::wstring(toConvert.begin(), toConvert.end());
	LPCWSTR sw = stemp.c_str();

	return sw;
}
