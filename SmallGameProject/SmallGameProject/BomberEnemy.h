#ifndef BOMBERENEMY_H
#define BOMBERENEMY_H

#include "Enemy.h"

static float EXPLOSION_DELAY = 5.0f;

class BomberEnemy : public Enemy {

private:

public:
    BomberEnemy(float posX, float posZ);
    virtual~BomberEnemy();

	bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere);
	void Shutdown();
	virtual bool ChargeExplosion(float deltaTime);

	virtual void Fire(float deltaTime);

};

#endif