#include "GameStateHandler.h"

GameStateHandler::GameStateHandler()
{
	m_gameStates.reserve(5);
}

GameStateHandler::~GameStateHandler()
{
}

void GameStateHandler::Shutdown()
{
	while (m_gameStates.size())
	{
		GameState* state = m_gameStates.back();
		if (!state->GetManualClearing())
		{
			state->Shutdown();
			delete state;
			state = NULL;
		}
		m_gameStates.pop_back();
	}
	while (m_popStates.size())
	{
		GameState* state = m_popStates.top();
		state->Shutdown();
		delete state;
		state = NULL;
		m_popStates.pop();
	}
}

bool GameStateHandler::Initialize(GraphicHandler* gHandler)
{
	int result = 1;
	/*StartState* startState = new StartState();
	startState->Initialize(gHandler, this);
	this->m_gameStates.push_back(startState);*/
	BilliardState* billiardState = new BilliardState();
	billiardState->Initialize(gHandler, this);
	this->m_gameStates.push_back(billiardState);
	return true;
}

int GameStateHandler::HandleInput(InputHandler * input)
{
	int result = 0;
	if (this->m_gameStates.size())
	{
		result = m_gameStates.back()->HandleInput(input);
	}
	return result;
}

int GameStateHandler::Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler)
{
	int result = 0;
	//Firstly pop the states that wanted to be cleared
	while (this->m_popStates.size())
	{
		GameState* state = this->m_popStates.top();
		state->Shutdown();
		delete state;
		state = NULL;
		this->m_popStates.pop();
	}
	//Check so that our states isn't empty
	if (this->m_gameStates.size())
	{
		//Update the active state
		result = m_gameStates.back()->Update(deltaTime, input, gHandler);
	}
	return result;
}


int GameStateHandler::Render(GraphicHandler * gHandler, HWND hwnd)
{
	int result = 0;
	if (m_gameStates.size())
	{
		result = m_gameStates.back()->Render(gHandler, hwnd);
	}
	return result;
}

void GameStateHandler::PushState(GameState * gameState)
{
	m_gameStates.push_back(gameState);
}

GameState * GameStateHandler::PopState()
{
	GameState* gameState = nullptr;
	//Check for size
	if (m_gameStates.size())
	{
		//Extract the gameState
		gameState = m_gameStates.back();
		m_gameStates.pop_back();
	}
	//If we could extract a gameState
	if (gameState)
	{
		//If the gameState wants to be automatically cleared by the system
		if (!gameState->GetManualClearing())
		{
			//Set the state to be popped at the end of the current/next update
			this->m_popStates.push(gameState);
			gameState = nullptr;
		}
	}
	return gameState;
}
