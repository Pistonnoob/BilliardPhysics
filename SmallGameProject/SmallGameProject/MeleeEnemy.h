#ifndef MELEEENEMY_H
#define MELEEENEMY_H

#include "Enemy.h"

static float M_ATTACK_DELAY = 3.0f;

class MeleeEnemy : public Enemy {

private:

public:
    MeleeEnemy(float posX, float posZ);
    virtual~MeleeEnemy();

	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere);
	void Shutdown();

	virtual void Fire(float deltaTime);
};

#endif