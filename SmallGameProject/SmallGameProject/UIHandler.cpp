#include "UIHandler.h"
#include "GraphicHandler.h"

UIHandler::UIHandler()
{
	this->elements = std::vector<UIElement>();
	this->textH = TextHandler();
}

UIHandler::~UIHandler()
{
}

bool UIHandler::Initialize(GraphicHandler* graphicsH)
{
	this->device = graphicsH->GetDevice();
	this->deviceContext = graphicsH->GetDeviceContext();

	this->screenWidth = graphicsH->GetScreenWidth();
	this->screenHeight = graphicsH->GetScreenHeight();

	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR camPos = DirectX::XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f);
	DirectX::XMVECTOR camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	this->viewMatrix = DirectX::XMMatrixLookAtLH(camPos, lookAt, camUp);

	this->projectionMatrix = graphicsH->GetOrthograpicMatrix();

	if(!this->textH.Initialize(this->device, this->deviceContext, this->viewMatrix, this->screenWidth, this->screenHeight)){
		return false;
	}

	

	return true;
}

void UIHandler::HandleInput(InputHandler * inputH)
{
	for (std::vector<UIElement>::iterator element = this->elements.begin(); element != this->elements.end(); element++) {
		(*element).UpdateClicked(inputH->getMousePos(), this->screenWidth, this->screenHeight);
	}
}

void UIHandler::RenderText(FontShaderHandler * fontShaderH)
{
	int indexCount;
	ID3D11ShaderResourceView* texture = nullptr;
	DirectX::XMFLOAT3 color;

	int textsToRender = this->textH.GetNrOfSentences();
	for (int i = 0; i < textsToRender; i++) {
		this->textH.RenderSentence(this->deviceContext, i, indexCount, texture, color);

		fontShaderH->Render(this->deviceContext, indexCount, DirectX::XMMatrixIdentity(), this->viewMatrix, this->projectionMatrix, texture, color);
	}
}

void UIHandler::RenderElements(UIShaderHandler * uiShaderH)
{
	for (auto element : this->elements) {
		element.Render(this->deviceContext);

		uiShaderH->Render(this->deviceContext, 6, element.GetWorldMatrix(), this->viewMatrix, this->projectionMatrix, element.GetTexture());
	}
}

bool UIHandler::WasButtonPressed(int buttonID)
{
	if (buttonID < this->elements.size()) {
		return this->elements.at(buttonID).WasClicked();
	}

	return false;
}

void UIHandler::Shutdown()
{
	for (auto element : this->elements) {
		element.Shutdown();
	}

	this->textH.Shutdown();
}

int UIHandler::AddElement(int width, int height, int posX, int posY, std::string textureMtl, int nrOfTextures, bool isButton)
{
	UIElement temp = UIElement();
	bool result = temp.Initialize(this->device, this->deviceContext, width, height, this->screenWidth, this->screenHeight, posX, posY, textureMtl, nrOfTextures, isButton);
	if (!result) {
		return -1;
	}
	this->elements.push_back(temp);
	
	return this->elements.size() - 1;
}

int UIHandler::UpdateElement(int elementIndex, int posX, int posY, int textureIndex)
{
	if (elementIndex >= this->elements.size()) {
		return -1;
	}

	this->elements.at(elementIndex).SetPosition(posX, posY, this->screenWidth, this->screenHeight);
	this->elements.at(elementIndex).ChangeTexture(textureIndex);

	return 1;
}

int UIHandler::RemoveElement(int elementIndex)
{
	if (elementIndex >= this->elements.size()) {
		return -1;
	}

	this->elements.erase(this->elements.begin() + elementIndex);
}

int UIHandler::CreateTextHolder(int maxLength)
{
	return this->textH.CreateSentence(this->device, maxLength);
}

bool UIHandler::UpdateTextHolder(int id, const std::string & text, int posX, int posY, const DirectX::XMFLOAT3 & color, float size)
{
	return this->textH.UpdateSentence(this->deviceContext, id, text, posX, posY, color, size);
}