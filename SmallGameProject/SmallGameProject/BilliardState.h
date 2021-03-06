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
	const float FRICTIONCOEFFICIENT_BETWEEN_BALL_AND_VELVET = 0.2f;
	const float GRAVITY = 9.82f;
	//const float SCALING = 1.0f;
	const float CUEBALL_RADIUS = 0.0476f;
	const float OTHERBALL_RADIUS = 0.0508f;
	//const float OTHERBALL_RADIUS = 0.0508f;
	const float SCALING = CUEBALL_RADIUS / 4.0f;
	//The width it SHOULD be
	//const float BOARD_WIDTH = 2.4f;
	//The width of the board model.... it is wrong I know... shh
	const float BOARD_WIDTH = 266.0f * SCALING;
	//const float BOARD_HEIGHT = BOARD_WIDTH / 2;
	const float BOARD_HEIGHT = 140.0f * SCALING;
	static const int OTHER_BALL_COUNT = 1;
	static const int CATCHER_COUNT = 6;
	const float COLLISION_RESTITUTION = 0.95f;
	const float NOT_MOVING_FLOAT = 0.000001f;
	struct Ball {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 direction;
		float density;
		float mass;
		float radius;
	};

	struct Catcher {
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 3.0f, 0.0f);
		float radius;
		float height = 8.0f;
	};

	Catcher billiardCatchers[CATCHER_COUNT];

	Ball activeBall;
	Ball otherBalls[OTHER_BALL_COUNT];
	//0 = caught. 1 = in play.
	int ballState[OTHER_BALL_COUNT];
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