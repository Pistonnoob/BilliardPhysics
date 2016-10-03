#ifndef STARTSTATE_H
#define STARTSTATE_H
#include "MenuState.h"
#include "Model.h"

class StartState :
	public GameState
{
private:
	bool startGame = false;
	Model startModel;
	CameraHandler camera;
public:
	StartState();
	StartState(GameStateHandler* GSH);
	virtual ~StartState();

	void Shutdown();

	int Initialize(GraphicHandler* gHandler, GameStateHandler* GSH);

	virtual int HandleInput(InputHandler* input);
	virtual int Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler);
	virtual int Render(GraphicHandler* gHandler, HWND hwnd);

private:
};

#endif