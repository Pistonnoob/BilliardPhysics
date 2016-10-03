#pragma once
#include "GameState.h"
class TestState :
	public GameState
{
public:
	TestState();
	~TestState();

	void Initialize();

	int Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameStateHandler* GSH);

	virtual int HandleInput(InputHandler* input);
	virtual int Update(float deltaTime);
	virtual int Render(GraphicHandler* gHandler, HWND hwnd);

};

