#ifndef UIHANDLER_H
#define UIHANDLER_H

#include <vector>

#include "UIElement.h"
#include "InputHandler.h"
#include "TextHandler.h"
#include "UIShaderHandler.h"
#include "FontShaderHandler.h"

class GraphicHandler;

class UIHandler
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	int screenWidth;
	int screenHeight;

	std::vector<UIElement> elements;
	TextHandler textH;

	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
public:
	UIHandler();
	~UIHandler();

	bool Initialize(GraphicHandler* graphicsH);
	void HandleInput(InputHandler* inputH);
	void RenderText(FontShaderHandler* fontShaderH);
	void RenderElements(UIShaderHandler* uiShaderH);
	void Shutdown();

	int AddElement(int width, int height, int posX, int posY, std::string textureMtl, int nrOfTextures, bool isButton = false);
	int UpdateElement(int elementIndex, int posX = -1, int posY = -1, int textureIndex = -1);
	int RemoveElement(int elementIndex);

	int CreateTextHolder(int maxLength);
	bool UpdateTextHolder(int id, const std::string& text, int posX, int posY, const DirectX::XMFLOAT3& color, float size);

	bool WasButtonPressed(int buttonID);
};

#endif