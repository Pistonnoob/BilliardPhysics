#ifndef FONT_H
#define FONT_H

#include "Texture.h"


class Font
{
private:
	struct FontChar
	{
		float left, right;
		int size;
	};
	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	FontChar* fontChars;
	Texture* fontTexture;

	bool LoadFontData(std::string fontFilename);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string textureFilename);
	void ReleaseTexture();

public:
	Font();
	~Font();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void* vertices, const char* text, float drawPosX, float drawPosY, float size);
};

#endif
