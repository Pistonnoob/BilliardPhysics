#include "Font.h"

Font::Font()
{
	this->fontChars = nullptr;
	this->fontTexture = nullptr;
}

Font::~Font()
{
}

bool Font::LoadFontData(std::string fontFilename)
{
	std::ifstream file;

	char junkChar;
	int junkInt;
	std::stringstream ss;
	std::string line;
	std::string path = "..\\SmallGameProject\\Resources\\Data\\" + fontFilename;
	file.open(path, std::ios::in);
	if (!file.is_open()) {
		return false;
	}

	this->fontChars = new FontChar[95];

	for (int i = 0; i < 95; i++) {
		std::getline(file, line);
		ss.str(line);
		ss >> junkInt >> junkChar >> this->fontChars[i].left >> this->fontChars[i].right >> this->fontChars[i].size;
		ss.clear();
	}


	file.close();

	return true;
}

void Font::ReleaseFontData()
{
	if (this->fontChars) {
		delete[] this->fontChars;
		this->fontChars = nullptr;
	}

	return;
}

bool Font::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string textureFilename)
{
	bool result;
	this->fontTexture = new Texture;
	if (!this->fontTexture) {
		return false;
	}

	result = this->fontTexture->Initialize(device, deviceContext, textureFilename);
	if (!result) {
		return false;
	}
	return true;
}

void Font::ReleaseTexture()
{
	if (this->fontTexture) {
		this->fontTexture->Shutdown();
		delete this->fontTexture;
		this->fontTexture = nullptr;
	}

	return;
}

bool Font::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* fontFilename, char* textureFilename)
{
	bool result;

	result = this->LoadFontData(fontFilename);
	if (!result) {
		return false;
	}

	result = this->LoadTexture(device, deviceContext, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}
void Font::Shutdown()
{
	this->ReleaseFontData();

	this->ReleaseTexture();

	return;
}

ID3D11ShaderResourceView* Font::GetTexture()
{
	return this->fontTexture->GetTexture(0);
}

void Font::BuildVertexArray(void* vertices, const char* text, float drawPosX, float drawPosY, float size)
{
	Vertex* verticesPtr = (Vertex*)vertices;
	int nrLetters = (int)strlen(text);
	int letter = 0;
	int index = 0;
	float dx, dy = 0.0f;

	for (int i = 0; i < nrLetters; i++) {
		letter = text[i] - 32;

		if (letter == 0) {
			drawPosX += (3.0f * size);
		}
		else {
			dx = float(this->fontChars[letter].size * size);
			dy = (16.0f * size);
			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX, drawPosY - dy, 0.0f); //Bottom left
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].left, 1.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX + dx, drawPosY, 0.0f); //Top right
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].right, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX, drawPosY, 0.0f); //Top left
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].left, 0.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX, drawPosY - dy, 0.0f); //Bottom left
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].left, 1.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX + dx, drawPosY - dy, 0.0f); //Bottom right
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].right, 1.0f);
			index++;

			verticesPtr[index].position = DirectX::XMFLOAT3(drawPosX + dx, drawPosY, 0.0f); //Top right
			verticesPtr[index].texture = DirectX::XMFLOAT2(this->fontChars[letter].right, 0.0f);
			index++;

			drawPosX += (dx + (1.0f * size));
		}
	}

	return;
}