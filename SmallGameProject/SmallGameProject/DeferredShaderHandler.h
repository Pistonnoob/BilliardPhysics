#ifndef DEFERREDSHADERHANDLER_H
#define DEFERREDSHADERHANDLER_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "ShaderStructLibrary.h"

const int BUFFER_COUNT = 5;

class DeferredShaderHandler {
private:
	struct CBPerObj
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;

		DirectX::XMFLOAT4 diffColor;
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 specColor;

		DirectX::XMFLOAT4 camPos;

		BOOL hasTexture;
	};

	ID3D11VertexShader* vertexShader;
	ID3D11GeometryShader* geoShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* samplerState;

	ID3D11RenderTargetView* deferredRenderTargetViews[BUFFER_COUNT];
	ID3D11Texture2D* deferredRenderTargetTextures[BUFFER_COUNT];
	ID3D11ShaderResourceView* deferredShaderResources[BUFFER_COUNT];

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND* hwnd, WCHAR* shaderFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, DeferredShaderParameters* params);

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart);

public:
	DeferredShaderHandler();
	~DeferredShaderHandler();

	bool Initialize(ID3D11Device* device, HWND* hwnd, int screenWidth, int screenHeight);
	void Shutdown();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, int indexStart, DeferredShaderParameters* params);

	void SetDeferredRenderTargets(ID3D11DeviceContext* deviceContext);
	void ClearRenderTargets(ID3D11DeviceContext* deviceContext);

	int GetBufferCount();

	ID3D11ShaderResourceView** GetShaderResourceViews();

	ID3D11DepthStencilView* GetDepthView();
};

#endif