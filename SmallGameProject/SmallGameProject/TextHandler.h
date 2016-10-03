#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include "Texture.h"
#include "Font.h"

class TextHandler {

public:
	struct Sentence {
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount;
		int indexCount;
		int maxLength;
		DirectX::XMFLOAT3 color;

		Sentence()
		{
			this->vertexBuffer = nullptr;
			this->indexBuffer = nullptr;
		}

		void ShutdownBuffers()
		{
			//Release the index buffer
			if (this->indexBuffer) {
				this->indexBuffer->Release();
				this->indexBuffer = nullptr;
			}

			//Release the vertex buffer
			if (this->vertexBuffer) {
				this->vertexBuffer->Release();
				this->vertexBuffer = nullptr;
			}
		}
	};

	struct Vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

private:
	DirectX::XMMATRIX baseViewMatrix;
	Font* font;
	std::vector<Sentence*> sentences;
	int screenWidth;
	int screenHeight;

	void ReleaseSentences();

public:
	TextHandler();
	TextHandler(const TextHandler&);
	~TextHandler();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX baseViewMatrix, int screenWidth, int screenHeight);
	void Shutdown();
	void RenderSentence(ID3D11DeviceContext* deviceContext, int id, int& indexCount, ID3D11ShaderResourceView*& texture, DirectX::XMFLOAT3& color);
	int GetNrOfSentences();
	int CreateSentence(ID3D11Device* device, int maxLength);
	bool UpdateSentence(ID3D11DeviceContext* deviceContext, int id, const std::string& text, int newPosX, int newPosY, const DirectX::XMFLOAT3& color, float size);
};



#endif