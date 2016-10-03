#ifndef RANGEDENEMY_H
#define RANGEDENEMY_H

#include "Enemy.h"

static float R_ATTACK_DELAY = 0.5f;

class RangedEnemy : public Enemy {

private:

public:
    RangedEnemy(float posX, float posZ);
    virtual~RangedEnemy();

	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere);
	void Shutdown();

    virtual void Fire(float deltaTime);
};

#endif