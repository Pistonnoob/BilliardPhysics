#ifndef SCREENQUAD_H
#define SCREENQUAD_H

#include <d3d11.h>
#include <DirectXMath.h>

class ScreenQuad {
private:
	struct VertexSimple {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
	};

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
public:
	ScreenQuad();
	~ScreenQuad();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight);
	void Render(ID3D11DeviceContext* deviceContext);
	void Shutdown();
};

#endif