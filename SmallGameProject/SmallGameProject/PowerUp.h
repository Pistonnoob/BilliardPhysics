#ifndef POWERUP_H
#define POWERUP_H
#include "Entity.h"

class PowerUp : public Entity
{
private:
	float timeLeft;
	Events::UNIQUE_FIRE type;
public:
	PowerUp(Events::UNIQUE_FIRE type);
	virtual ~PowerUp();

	virtual void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string powerUpModelFilename, bool isSphere, EntitySubject* entitySub);

	float GetTimeLeft() const;
	void SetTimePowerup(float timeActive);
	Events::UNIQUE_FIRE GetType() const;


	virtual bool Update(float dt);
};

#endif

