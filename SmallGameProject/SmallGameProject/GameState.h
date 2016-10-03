#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "InputHandler.h"
#include <Windows.h>
#include "GraphicHandler.h"
#include <d3d11.h>
class GameStateHandler;
class GameState {

protected:
	GameStateHandler* m_GSH;
	bool manualClearing;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	std::vector<PointLight> pointLights;
public:

	GameState();	//This will initialize the GSH pointer to NULL
	virtual ~GameState();

	virtual void Shutdown();

	int InitializeBase(GameStateHandler * GSH, ID3D11Device * device, ID3D11DeviceContext * deviceContext);

	virtual int Initialize(GraphicHandler* gHandler, GameStateHandler* GSH) = 0;
	
	bool GetManualClearing();
	void SetManualClearing(bool myCase);

	virtual int HandleInput(InputHandler* input) = 0;
	virtual int Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler) = 0;
	virtual int Render(GraphicHandler* gHandler, HWND hwnd) = 0;
	
};

#endif