#include "D3DHandler.h"
#include <iostream>
#include <assert.h> 

D3DHandler::D3DHandler()
{
	this->StartUpValues();
}

D3DHandler::~D3DHandler()
{
	this->Shutdown();
}

void D3DHandler::SetWindowToEngine(HWND &setWindow) throw(...)
{
	this->activeWindow = &setWindow;
	if (this->activeWindow == nullptr)
	{
		throw("error in setWindowToEngine");
	}
}

bool D3DHandler::Initialize(HWND* window, int clientWidth, int clientHeight) throw(...)
{
	std::string errorMessage;
	bool result = true;
	HRESULT resultHelper;

	this->activeWindow = window;
	this->clientWidth = clientWidth;
	this->clientHeight = clientHeight;
	this->clientDriverType = D3D_DRIVER_TYPE_HARDWARE;
	this->featureSupport = D3D_FEATURE_LEVEL_11_0;

	if (this->activeWindow == nullptr)
	{
		errorMessage = "Can not initialize the D3DHandler, no active window is set";
		result = false;
	}

	try
	{
		this->CreateDeviceAndContext();
		this->Check4xMsaaQualitySupp();
		
		DXGI_SWAP_CHAIN_DESC scDesc;
		scDesc = this->DescribeSwapChain();

		this->CreateSwapChain(&scDesc);
		this->CreateRenderTargetViewDS();
		this->CreateRasterizerState();
		this->CreateDepthBufferAndView();
		this->CreateStencilStates();
	}

	catch (char* errorMessage)
	{
		std::cout << errorMessage << std::endl;
		result = false;
	}

	//if all is well, bind the view to output merger stage
	if (result == true)
	{
		this->gDeviceContext->OMSetRenderTargets
			(
				1,
				&this->backBufferRTV,
				this->mDepthStencilView
			);

		this->SetInitialViewPort();
	}

	return result;
}

ID3D11Device * D3DHandler::GetDevice() const
{
	return this->gDevice;
}

ID3D11DeviceContext * D3DHandler::GetDeviceContext() const
{
	return this->gDeviceContext;
}

void D3DHandler::ClearDepthAndRTVViews()
{
	float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	this->gDeviceContext->ClearRenderTargetView
		(
			this->backBufferRTV,
			black
		);

	this->gDeviceContext->ClearDepthStencilView
		(
			this->mDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0
		);
}

void D3DHandler::SetDepth(const int &desired) //1 = enable, 2 = disable, 3 = enable -write
{
	if (desired == 1)
	{
		this->gDeviceContext->OMSetDepthStencilState(this->enableDepth, 1);
	}
	else if(desired == 2)
	{
		this->gDeviceContext->OMSetDepthStencilState(this->disableDepth, 1);
	}
	else if (desired == 3) {
		this->gDeviceContext->OMSetDepthStencilState(this->particleDepth, 1);
	}
}

void D3DHandler::SetRenderTargetView(ID3D11DepthStencilView* depthView)
{
	if (depthView) {
		this->gDeviceContext->OMSetRenderTargets(1, &this->backBufferRTV, depthView);
	}
	else {
		this->gDeviceContext->OMSetRenderTargets(1, &this->backBufferRTV, this->mDepthStencilView);
	}
}

void D3DHandler::PresentScene()
{
	this->gSwapChain->Present(0, 0);
}

void D3DHandler::SetViewport()
{
	this->GetDeviceContext()->RSSetViewports(1, &this->gameViewport);
}

void D3DHandler::CreateDeviceAndContext() throw(...)
{
	HRESULT resultHelper;
	bool result = true;
	D3D_FEATURE_LEVEL featureLevel;
	std::string errorMessage;


	resultHelper = D3D11CreateDevice(
		0,							// default adapter
		this->clientDriverType,		// D3D_DRIVER_TYPE_HARDWARE, created in the default constructor
		0,							// no software device
		D3D11_CREATE_DEVICE_DEBUG,	// flag
		0, 
		0,						// default feature level array
		D3D11_SDK_VERSION,
		&this->gDevice,
		&featureLevel,
		&this->gDeviceContext);

	if (FAILED(resultHelper))
	{
		errorMessage = "D3D11CreateDevice Failed.";
		result = false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		errorMessage = "Direct3D Feature Level 11 unsupported. I messed up featurelevel somehow";
		result = false;
	}
	if (result == false)
	{
		throw(errorMessage);
	}
}

void D3DHandler::Check4xMsaaQualitySupp() throw(...)
{
	std::string errorMessage = "Check4XMSAA quality support function error";
	UINT sampleCount = 4;
	HRESULT resultHelper;

	resultHelper = this->gDevice->CheckMultisampleQualityLevels
		(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			sampleCount,
			&this->m4xMsaaQuality
		);

	if(FAILED(resultHelper))
	{
		throw(errorMessage);
	}
	assert(m4xMsaaQuality > 0);
}

DXGI_SWAP_CHAIN_DESC D3DHandler::DescribeSwapChain() throw(...)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = this->clientWidth;
	swapChainDesc.BufferDesc.Height = this->clientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	//pc master-race
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//are we using MSAA?
	if (this->msaa4xEnabled)
	{
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = this->m4xMsaaQuality - 1;
	}
	//if no MSAA
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = *this->activeWindow;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	return swapChainDesc;
}

bool D3DHandler::CreateSwapChain(DXGI_SWAP_CHAIN_DESC* desc) throw(...)
{
	HRESULT resultHelper;
	bool result = true;
	std::string errorMessage = "";

	IDXGIDevice* dxgiDevice = nullptr;
	resultHelper = this->gDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	
	if (FAILED(resultHelper))
	{
		errorMessage = "DxGiDevice error";
		result = false;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	resultHelper = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	if (FAILED(resultHelper))
	{
		errorMessage = "DxGiDevice error";
		result = false;
	}

	//get the IDXGIFactory interface
	IDXGIFactory* dxgiFactory = nullptr;
	resultHelper = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	if (FAILED(resultHelper))
	{
		errorMessage = "DxGiDevice error";
		result = false;
	}

	//create the swapchain
	resultHelper = dxgiFactory->CreateSwapChain(this->gDevice, desc, &this->gSwapChain);

	if (FAILED(resultHelper))
	{
		errorMessage = "error creating the swapchain";
		result = false;
	}

	return result;
}

void D3DHandler::CreateRenderTargetViewDS() throw(...)
{
	HRESULT resultHandler;
	ID3D11Texture2D* backBuffer;
	this->gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));

	resultHandler = this->gDevice->CreateRenderTargetView(backBuffer, 0, &this->backBufferRTV);

	if (FAILED(resultHandler))
	{
		throw("Failed to create the render target view for the depth buffer");
	}

	//realease the com object
	backBuffer->Release();
}

void D3DHandler::CreateDepthBufferAndView() throw(...)
{
	HRESULT resultHandler;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = this->clientWidth;
	depthStencilDesc.Height = this->clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//if msaa is enabled
	if (this->msaa4xEnabled == true)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = this->m4xMsaaQuality - 1;
	}

	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	resultHandler = gDevice->CreateTexture2D
		(
		&depthStencilDesc,
		nullptr,
		&this->mDepthStencilBuffer
		);

	if (FAILED(resultHandler))
	{
		throw("Cant create the 2D texture for the depthbuffer");
	}

	resultHandler = gDevice->CreateDepthStencilView
		(
			this->mDepthStencilBuffer,
			nullptr,
			&this->mDepthStencilView
		);
	if (FAILED(resultHandler))
	{
		throw("Create DepthStencilView");
	}
}

void D3DHandler::CreateStencilStates() throw(...)
{
	HRESULT resultHelper;

	D3D11_DEPTH_STENCIL_DESC enableDepth;
	enableDepth.DepthEnable = true;
	enableDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // enables writes to the depth buffer
	enableDepth.DepthFunc = D3D11_COMPARISON_LESS; //usual depth test is preformed

	//FrontFace indicates how the stencil buffer works for front facing triangles
	//Direct3D, Luna page 376
	enableDepth.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	enableDepth.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	enableDepth.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	enableDepth.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//BackFace indicates how the stencil buffer works for back facing triangles
	//Direct3D, Luna page 376
	enableDepth.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	enableDepth.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	enableDepth.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	enableDepth.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	resultHelper = this->gDevice->CreateDepthStencilState(&enableDepth, &this->enableDepth);
	if (FAILED(resultHelper))
	{
		throw("failed to create the 'enable Depth' stencil state");
	}

	//enabled but no new write
	enableDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disables writes to the depth buffer

	resultHelper = this->gDevice->CreateDepthStencilState(&enableDepth, &this->particleDepth);
	if (FAILED(resultHelper))
	{
		throw("failed to create the 'enable Depth' stencil state");
	}

	//same as above but disable depth
	enableDepth.DepthEnable = false;

	resultHelper = this->gDevice->CreateDepthStencilState(&enableDepth, &this->disableDepth);

	if (FAILED(resultHelper))
	{
		throw("failed to create the 'disable Depth' stencil state");
	}
}

void D3DHandler::SetInitialViewPort()
{
	this->gameViewport.TopLeftX = 0.0f;
	this->gameViewport.TopLeftY = 0.0f;
	this->gameViewport.Width = static_cast<float>(this->clientWidth);
	this->gameViewport.Height = static_cast<float>(this->clientHeight);
	this->gameViewport.MinDepth = 0.0f;
	this->gameViewport.MaxDepth = 1.0f;

	this->gDeviceContext->RSSetViewports(1, &this->gameViewport);
}

void D3DHandler::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterDesc;

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));
	//Setup the resterizer state for more rendering control
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//Create the resterizer state from description
	this->gDevice->CreateRasterizerState(&rasterDesc, &this->rasterState);

	//Set the rasterizer state
	this->gDeviceContext->RSSetState(this->rasterState);
}

void D3DHandler::StartUpValues()
{
	this->gDevice			= nullptr;
	this->gDeviceContext	= nullptr;
	this->activeWindow		= nullptr;
	this->gSwapChain		= nullptr;

	this->mDepthStencilBuffer	= nullptr;
	this->backBufferRTV = nullptr;
	this->mDepthStencilView		= nullptr;

	this->disableDepth = nullptr;
	this->enableDepth = nullptr;
}

HWND * D3DHandler::GetActiveWindow()
{
	return this->activeWindow;
}

void D3DHandler::Shutdown()
{
	if (this->enableDepth) {
		this->enableDepth->Release();
		this->enableDepth = nullptr;
	}
	if (this->disableDepth) {
		this->disableDepth->Release();
		this->disableDepth = nullptr;
	}

	if (this->mDepthStencilBuffer) {
		this->mDepthStencilBuffer->Release();
		this->mDepthStencilBuffer = nullptr;
	}
	if (this->mDepthStencilView) {
		this->mDepthStencilView->Release();
		this->mDepthStencilView = nullptr;
	}

	if (this->rasterState) {
		this->rasterState->Release();
		this->rasterState = nullptr;
	}

	if (this->backBufferRTV) {
		this->backBufferRTV->Release();
		this->backBufferRTV = nullptr;
	}

	if (this->gSwapChain) {
		this->gSwapChain->Release();
		this->gSwapChain = nullptr;
	}

	if (this->gDeviceContext) {
		this->gDeviceContext->Release();
		this->gDeviceContext = nullptr;
	}
	if (this->gDevice) {
		this->gDevice->Release();
		this->gDevice = nullptr;
	}
}