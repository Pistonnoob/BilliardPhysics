#ifndef ShadowShaderHandler_H
#define ShadowShaderHandler_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <string>

#include "ShaderStructLibrary.h"

class ShadowShaderHandler {
private:
	struct ShadowConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* layout;	
	ID3D11Buffer* matrixBuffer;		
	
	D3D11_VIEWPORT* viewPort; 

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;
	ID3D11Texture2D* depthMap;

	ID3D10Blob* vertexShaderBuffer;

	static int const nrOfShaderResources = 1;
public:
	ShadowShaderHandler();
	virtual ~ShadowShaderHandler();

	bool Initialize(ID3D11Device* gDevice, HWND* hwnd, int nrOfResources, int screenWidth, int screenHeight) throw(...);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* gDeviceContext);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart, ShadowShaderParameters* params);
	void ClearShadowMap(ID3D11DeviceContext* gDeviceContext);

	//getters / setters
	ID3D11ShaderResourceView* getShadowMapSRW() const;
	void SetViewPort(ID3D11DeviceContext* gDeviceContext);


private:
	void StartUp();
	

	void Create2DTexture(ID3D11Device* gDevice, int screenWidth, int screenHeight) throw(...);
	void CreateDepthStencilView(ID3D11Device* gDevice) throw(...);
	void CreateShaderResourceView(ID3D11Device* gDevice) throw(...);

	/*
	vertex shader help functions
	*/
	void LoadVertexShaderFromFile() throw(...);
	void CreateVertexLayout(ID3D11Device* gDevice) throw(...);
	void CreateVertexShader(ID3D11Device* gDevice) throw(...);
	void CreateConstantBuffer(ID3D11Device* gDevice) throw(...);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, ShadowShaderParameters* params) throw(...);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart);

	LPCWSTR stringToLPCSTR(std::string toConvert) const;
};


#endif