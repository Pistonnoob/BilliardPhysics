#include "Enemy.h"

Enemy::Enemy()
: Actor()
{
}

Enemy::~Enemy()
{
}


void Enemy::MoveRight(float deltaTime)
{
    this->posX += MOVEMENT_SPEED * deltaTime;
}
void Enemy::MoveLeft(float deltaTime)
{
    this->posX -= MOVEMENT_SPEED * deltaTime;
}
void Enemy::MoveUp(float deltaTime)
{
    this->posZ += MOVEMENT_SPEED * deltaTime;
}
void Enemy::MoveDown(float deltaTime)
{
	this->posZ -= MOVEMENT_SPEED * deltaTime;
}
void Enemy::Fire(float deltaTime)
{
    if (this->attackCD > BASE_ATTACK_DELAY)
    {
        this->entitySubject->Notify(this, Events::ENTITY::Fire);
        this->attackCD = 0;
    }
    else
    {
        this->attackCD += deltaTime;
    }
}
void Enemy::Move(DirectX::XMFLOAT3 moveVec)
{
    this->posX += moveVec.x;
    this->posZ += moveVec.z;
}