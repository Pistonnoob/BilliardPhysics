#ifndef GAMESTATEHANDLER_H
#define GAMESTATEHANDLER_H
#include "InputHandler.h"
#include "GraphicHandler.h"
#include <d3d11.h>
#include <vector>
#include <stack>
#include <Windows.h>

#include "StartState.h"
class GameStateHandler {

private:
	std::vector<GameState*> m_gameStates;
	std::stack<GameState*> m_popStates;
public:
	GameStateHandler();
	~GameStateHandler();

	void Shutdown();

	bool Initialize(GraphicHandler* gHandler);

	int HandleInput(InputHandler* input);
	int Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler);
	int Render(GraphicHandler* gHandler, HWND hwnd);

	void PushState(GameState* gameState);
	GameState* PopState();
};


#endif
