#ifndef GAMESTRUCT_H
#define GAMESTRUCT_H

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <D3D11.h>
#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT4 Direction;
	float pad;
};

struct PointLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT4 Position;
	float Range;

	//controlls how light intensity falls of with distance
	DirectX::XMFLOAT4 Attenuation;
};

struct SpotLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Direction;
	float Spot;

	//controlls how light intensity falls of with distance
	DirectX::XMFLOAT4 Attenuation;
};
#endif