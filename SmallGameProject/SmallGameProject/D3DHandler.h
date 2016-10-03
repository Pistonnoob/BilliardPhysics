#ifndef D3DHANDLER_H
#define D3DHANDLER_H

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <D3D11.h>
#include <DirectXMath.h>
#include <windows.h>
#include <string>

class D3DHandler 
{
public:
	D3DHandler();
	virtual ~D3DHandler();

	void SetWindowToEngine(HWND &setWindow) throw(...);
	bool Initialize(HWND* window, int clientWidth, int clientHeight) throw(...);

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	void ClearDepthAndRTVViews();

	void SetDepth(const int &desired);
	void SetRenderTargetView(ID3D11DepthStencilView* depthView = nullptr);

	void PresentScene();
	void SetViewport();

	void Shutdown();

	HWND* GetActiveWindow();
protected:
	void CreateDeviceAndContext() throw(...);
	// checks the quality level support for 4x MSAA
	void Check4xMsaaQualitySupp() throw(...);
	
	//swapChain
	DXGI_SWAP_CHAIN_DESC DescribeSwapChain() throw(...);
	bool CreateSwapChain(DXGI_SWAP_CHAIN_DESC* desc) throw(...);

	//depth stencil
	void CreateRenderTargetViewDS() throw(...);
	void CreateDepthBufferAndView() throw(...);

	//setting the 2 stencil states
	void CreateStencilStates() throw(...);

	//set initial viewPort
	void SetInitialViewPort();

	void CreateRasterizerState();

	//setAllVariablestToNullPtr
	void StartUpValues();
	void CalculateFrameStats();
	
protected:
	ID3D11Device* gDevice;
	ID3D11DeviceContext* gDeviceContext;
	HWND* activeWindow;
	IDXGISwapChain* gSwapChain;

	//data to depthStencil
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* mDepthStencilView;

	//RenderStates
	ID3D11DepthStencilState* disableDepth;
	ID3D11DepthStencilState* enableDepth;
	ID3D11DepthStencilState* particleDepth;

	D3D11_VIEWPORT gameViewport;

	D3D_DRIVER_TYPE clientDriverType;
	D3D_FEATURE_LEVEL featureSupport;

	ID3D11RasterizerState* rasterState;
	
	int clientWidth;
	int clientHeight;

	UINT m4xMsaaQuality;
	bool msaa4xEnabled;
};

#endif