#ifndef ENEMY_H
#define ENEMY_H

#include "Actor.h"

static float BASE_ATTACK_DELAY = 0.5f;
static float MOVEMENT_SPEED = 10.0f;



class Enemy : public Actor {

protected:
    float attackCD;

public:
    Enemy();
    virtual~Enemy();

	virtual bool Initialize(Model* model, EntitySubject* entitySubject, bool isSphere) = 0;
	virtual void Shutdown() = 0;
	
    virtual void MoveRight(float deltaTime);
    virtual void MoveLeft(float deltaTime);
    virtual void MoveUp(float deltaTime);
    virtual void MoveDown(float deltaTime);
    virtual void Fire(float deltaTime);
    virtual void Move(DirectX::XMFLOAT3 moveVec) ;

};

#endif