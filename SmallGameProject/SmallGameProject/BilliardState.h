#ifndef BILLIARDSTATE_H
#define BILLIARDSTATE_H
#include "GameState.h"
#include "Model.h"
#include "MyMathLib.h"
class BilliardState :
	public GameState
{
private:

	bool exitStage;

	CameraHandler myCamera;

	Model m_cueBall;
	Model m_8Ball;
	Model m_cueStick; //Cue stick
	//The pelt is made of velvet
	Model m_table;

	bool simulationCompleted;
	const float PI = DirectX::XM_PI;
	const float FRICTIONCOEFFICIENT_BETWEEN_BALL_AND_VELVET = 0.005f;
	const float GRAVITY = 9.82;
	const float SCALING = 1.0f;
	const float BOARD_WIDTH = 2.4f;
	const float BOARD_HEIGHT = BOARD_WIDTH / 2;
	const int OTHER_BALL_COUNT = 1;

	struct Ball {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 velocity;
		float density;
		float mass;
		float radius;
	};

	struct Catcher {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 3.0f, 0.0f);
		float radius;
		float height = 8.0f;
	};

	Catcher billiardCatchers[6];

	Ball activeBall;
	Ball otherBalls[1];


public:
	BilliardState();
	~BilliardState();

	void Shutdown();

	int Initialize(GraphicHandler* gHandler, GameStateHandler* GSH);


	virtual int HandleInput(InputHandler* input);
	virtual int Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler);
	virtual int Render(GraphicHandler* gHandler, HWND hwnd);
};

#endif