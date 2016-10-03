#include "TestState.h"



TestState::TestState()
{
}


TestState::~TestState()
{
}

void TestState::Initialize()
{
}

int TestState::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, GameStateHandler * GSH)
{
	return 0;
}

int TestState::HandleInput(InputHandler * input)
{
	return 0;
}

int TestState::Update(float deltaTime)
{
	return 0;
}

int TestState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	return 0;
}
