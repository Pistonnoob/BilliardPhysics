#include "Particle.h"



Particle::Particle()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->r = this->g = this->b = this->uCoord = 0.0f;;
	this->velocity = 0.0f;
	//Not the true distance but close enough
	this->cameraDistance = 0.0f;
	this->time = 0.0f;
	this->active = false;

}


Particle::~Particle()
{
}
