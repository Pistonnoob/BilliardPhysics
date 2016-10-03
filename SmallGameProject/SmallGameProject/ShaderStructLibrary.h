#ifndef SHADERSTRUCTLIBRARY_H
#define SHADERSTRUCTLIBRARY_H

#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include "LightStructs.h"

struct DeferredShaderParameters {
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	ID3D11ShaderResourceView* diffTexture;

	DirectX::XMFLOAT4 diffColor;
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 specColor;
	DirectX::XMFLOAT4 camPos;
};

struct LightShaderParameters {
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX lightViewMatrix;
	DirectX::XMMATRIX lightProjectionMatrix;

	ID3D11ShaderResourceView** deferredTextures;
	ID3D11ShaderResourceView* shadowTexture;

	DirectX::XMFLOAT4 camPos;

	DirectionalLight dirLight;
	std::vector<PointLight> pointLights;
};

struct ParticleShaderParameters {
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	ID3D11ShaderResourceView* diffTexture;

	DirectX::XMFLOAT4 camPos;
};

struct ShadowShaderParameters {
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
};

#endif