#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class Texture {
public:
	struct Material
	{
		std::string name;
		DirectX::XMFLOAT4 diffColor;
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 specColor;
		bool hasTexture;
		int textureIndex;
		bool hasNormMap;
		int normMapIndex;
	};
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	unsigned char* targaData;
	ID3D11Texture2D* texture;
	std::vector<ID3D11ShaderResourceView*> textureViews;
	std::vector<Material> materials;
	std::vector<std::string> textureNames;

	bool LoadTarga(const char*, int&, int&);
	bool LoadMTL(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string& mtlFilename);
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string& mtlFilename);
	void Shutdown();

	int GetMaterialIndexFromName(std::string& materialName);
	Material GetMaterial(int materialIndex);
	ID3D11ShaderResourceView* GetTexture(int textureIndex);
	ID3D11ShaderResourceView* SwapTextureView(ID3D11ShaderResourceView* textureView, int textureIndex);
};

#endif