#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

#include "Texture.h"

class UIElement {
private:
	struct VertexSimple {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	DirectX::XMMATRIX worldMatrix;

	Texture texture;

	int width;
	int height;
	int posX;
	int posY;
	bool clickAble;
	bool wasClicked;
	int activeTexture;
	int nrOfTextures;
public:
	UIElement();
	~UIElement();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, int screenWidth, int screenHeight,
		int posX, int posY, std::string textureMtl, int nrOfTextures, bool clickAble);
	void Render(ID3D11DeviceContext* deviceContext);
	void Shutdown();

	void UpdateClicked(DirectX::XMFLOAT2 mousePos, int screenWidth, int screenHeight);
	bool WasClicked();

	void ChangeTexture(int textureIndex);
	ID3D11ShaderResourceView* GetTexture();

	void SetPosition(float posX, float posY, int screenWidth, int screenHeight);
	DirectX::XMMATRIX GetWorldMatrix();
};

#endif