#ifndef BILLIARDSTATE_H
#define BILLIARDSTATE_H
#include "GameState.h"
#include "Model.h"
class BilliardState :
	public GameState
{
private:
	CameraHandler myCamera;

	Model m_ballCue;
	Model m_ball8;
	Model m_cueStick; //Cue stick
	Model m_table;

	CameraHandler myCamera;

	bool ballsNotMoving;

public:
	BilliardState();
	~BilliardState();
};

#endif