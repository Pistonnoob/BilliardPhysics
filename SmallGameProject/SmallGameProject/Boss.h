#ifndef BOSS_H
#define BOSS_H


#include "Enemy.h"
#include "Abilities.h"

class Boss : public Enemy {

private:
	std::vector<Ability*> abilities;
	float attackDelay;
	int abilityUsed;
public:
	Boss(float posX, float posZ);
	virtual~Boss();

	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere);
	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere, int nrOfAbilities);
	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere, std::vector<Ability*> abilities);
	void Shutdown();

	virtual void update(DirectX::XMFLOAT3 playerPos, float deltaTime);

};

#endif
