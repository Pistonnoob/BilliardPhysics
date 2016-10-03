#ifndef LIGHTSHADERHANDLER_H
#define LIGHTSHADERHANDLER_H

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "ShaderStructLibrary.h"
#include "LightStructs.h"

const int MAX_LIGHTS = 10;

class LightShaderHandler {
private:
	struct LightMatrixBuffer
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;

		DirectX::XMFLOAT4 camPos;
		DirectX::XMFLOAT4 lightPos;
	};
	struct LightsCB
	{
		DirectX::XMFLOAT4 Ambient[MAX_LIGHTS];
		DirectX::XMFLOAT4 Diffuse[MAX_LIGHTS];
		DirectX::XMFLOAT4 Specular[MAX_LIGHTS];

		DirectX::XMFLOAT4 Position[MAX_LIGHTS];
		DirectX::XMFLOAT4 Attenuation[MAX_LIGHTS - 1];

		UINT activeLights;
		float padding[3];
	};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* samplerState;
	ID3D11SamplerState* shadowSamplerState;
	ID3D11ShaderResourceView** nullResource;
	int nrOfShaderResources;

	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND* hwnd, WCHAR* shaderFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, LightShaderParameters* params); 

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

public:
	LightShaderHandler();
	~LightShaderHandler();

	bool Initialize(ID3D11Device* device, HWND* hwnd, int nrOfResources);
	void Shutdown();
	void ResetPSShaderResources(ID3D11DeviceContext* deviceContext);

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, LightShaderParameters* params);
};

#endif