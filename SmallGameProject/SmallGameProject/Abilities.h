#ifndef ABILITIES
#define ABILITIES
#include "Ability.h"


class ArcFire : public Ability
{
private:

protected:

public:
	ArcFire();
	virtual~ArcFire();
	virtual void Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay);
	virtual float Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos);
	virtual void Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime);



};

class SplitFire : public Ability
{
private:

protected:
	int projectilesOnSplit;
	float splitArc;
public:
	SplitFire();
	virtual~SplitFire();
	virtual void Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay, int projectilesOnSplit, float splitArc);
	virtual float Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos);
	virtual void Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime);

};

class ReverseFire : public Ability
{
private:

protected:

public:
	ReverseFire();
	virtual~ReverseFire();
	virtual void Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay);
	virtual float Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos);
	virtual void Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime);

};

#endif
