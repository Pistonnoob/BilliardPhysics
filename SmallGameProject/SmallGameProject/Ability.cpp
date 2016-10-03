#include "Ability.h"
Ability::Ability()
{
	this->isActivated = false;
	this->counter = 0;
	this->cdCounter = 0;
	this->chargesLeft = 0;
	this->chargesRefresh = 0;
}
Ability::~Ability()
{
}
void Ability::Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay)
{
	this->arc = arc;
	this->nrOfProjectiles = nrOfProjectiles;
	this->cooldown = cooldown;
	this->attackDelay = attackDelay;
	this->maxCharges = maxCharges;
	this->chargesLeft = maxCharges;
	this->triggerDelay = triggerDelay;

}
void Ability::Update(Enemy * enemy, EntitySubject * entitySubject, float deltaTime)
{
	if (this->chargesLeft < this->maxCharges)
	{
		//this->cdCounter += REFRESH_RATE * deltaTime;
		//this->chargesRefresh += REFRESH_RATE * deltaTime;
		this->cdCounter += deltaTime;
		this->chargesRefresh += deltaTime;
	}

	if (this->chargesRefresh > this->cooldown && this->chargesLeft < this->maxCharges)
	{
		this->chargesRefresh = 0;
		this->chargesLeft++;
	}
}

float Ability::GetCooldown()
{
	return this->cooldown;
}

float Ability::GetCDCounter()
{
	return this->cdCounter;
}
