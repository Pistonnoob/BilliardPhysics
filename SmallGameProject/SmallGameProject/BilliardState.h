#ifndef BILLIARDSTATE_H
#define BILLIARDSTATE_H
#include "GameState.h"
#include "Model.h"
#include "MyMathLib.h"
class BilliardState :
	public GameState
{
private:
	CameraHandler myCamera;

	Model m_ballCue;
	Model m_ball8;
	Model m_cueStick; //Cue stick
	//The pelt is made of velvet
	Model m_table;

	CameraHandler myCamera;

	bool ballsNotMoving;
	const float PI = DirectX::XM_PI;
	const float FRICTIONCOEFFICIENT_BETWEEN_BALL_AND_VELVET = 0.005f;
	const float GRAVITY = 9.82;

	struct Ball {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 velocity;
		float density;
		float mass;
		float radius;
	};

public:
	BilliardState();
	~BilliardState();
};

#endif