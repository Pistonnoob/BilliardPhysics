#ifndef ABILITY_H
#define ABILITY_H
#include "Projectile.h"
#include "Model.h"
#include "Enemy.h"

static float REFRESH_RATE = 5.0f;

class Ability
{
private:

protected:
	std::vector<float> timers;
	bool isActivated;

	float arc;
	int nrOfProjectiles;
	float cooldown;
	int attackDelay;
	int maxCharges;
	float triggerDelay;

	float counter;
	float cdCounter;
	int chargesLeft;
	float chargesRefresh;

public:
	Ability();
	virtual~Ability();
	virtual void Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay);
	virtual float Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos) = 0;
	virtual void Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime);

	virtual float GetCooldown();
	virtual float GetCDCounter();




};
#endif
