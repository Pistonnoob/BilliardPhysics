#include "GameState.h"
#include "GameStateHandler.h"

GameState::GameState()
{
	m_GSH = NULL;
	manualClearing = false;
	m_device = nullptr;
	m_deviceContext = nullptr;
}

GameState::~GameState()
{
}

void GameState::Shutdown()
{
	this->m_GSH = NULL;
	this->m_device = NULL;
	this->m_deviceContext = NULL;
}

int GameState::InitializeBase(GameStateHandler * GSH, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	int result = 1;
	this->m_GSH = GSH;
	this->m_device = device;
	this->m_deviceContext = deviceContext;
	return result;
}

bool GameState::GetManualClearing()
{
	return this->manualClearing;
}

void GameState::SetManualClearing(bool myCase)
{
	this->manualClearing = myCase;
}