#ifndef PARTICLE_H
#define PARTICLE_H
//#include "Algorithm.h"
class Particle
{
public:
	//Variables
	float x, y, z, scale;
	float r, g, b, uCoord;
	float velocity;
	//Not the true distance but close enough
	float cameraDistance;
	float time;
	bool active;


public:
	//Functions
	Particle();
	virtual ~Particle();

	bool operator<(const Particle& that)const {
		//Sort in reverse order distance from camera > being optimal
		return this->cameraDistance > that.cameraDistance;
	}
};

#endif