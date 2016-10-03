#ifndef HUBSTATE_H
#define HUBSTATE_H
#include "GameState.h"
#include "ParticleHandler.h"
#include "Player.h"
#include "Algorithm.h"
#include "StageState.h"
#include "UIHandler.h"
#include <sstream>

const int NR_OF_MAPS = 2;

class HubState :
	public GameState
{
private:	//Variables
	CameraHandler myCamera;

	Model m_ground;
	std::vector<Model> portals;

	Player player;
	EntitySubject playerSubject;
	ParticleHandler myParticleHandler;
	UIHandler hubStatistics;

	bool renderPlayerStats;


	float t;

	DirectX::XMFLOAT3 playerPos;

	bool exitStage;

public:
	HubState();
	virtual ~HubState();
	void Shutdown();

	int Initialize(GraphicHandler* gHandler, GameStateHandler* GSH);

	virtual int HandleInput(InputHandler* input);
	virtual int Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler);
	virtual int Render(GraphicHandler* gHandler, HWND hwnd);

};

#endif