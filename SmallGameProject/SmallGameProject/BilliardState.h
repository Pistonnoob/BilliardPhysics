#ifndef BILLIARDSTATE_H
#define BILLIARDSTATE_H
#include "GameState.h"
#include "Model.h"
class BilliardState :
	public GameState
{
private:
	CameraHandler myCamera;

	Model m_ball;
	Model m_ground;
	Model m_cue; //Cue stick
	Model m_table;

public:
	BilliardState();
	~BilliardState();
};

#endif