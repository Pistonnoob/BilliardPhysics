#include "RangedEnemy.h"

RangedEnemy::RangedEnemy(float posX, float posZ)
{
    this->posX = posX;
    this->posZ = posZ;
    this->myType = Type::RANGED;
	this->health = 50;
	this->damage = 20;
}

RangedEnemy::~RangedEnemy()
{
}

bool RangedEnemy::Initialize(Model * model, EntitySubject* entitySubject, bool isSphere)
{
    this->aimDir = DirectX::XMFLOAT3(0, 0, 0);
	bool result = Entity::Initialize(model, entitySubject, isSphere);
	this->entitySubject->Notify(this, Events::ENTITY::RANGED_CREATED);
	return result;
}

void RangedEnemy::Shutdown()
{
	this->entitySubject->Notify(this, Events::ENTITY::RANGED_DEAD);
	Entity::Shutdown(true);
}


void RangedEnemy::Fire(float deltaTime)
{
    if (this->attackCD > R_ATTACK_DELAY)
    {
        this->entitySubject->Notify(this, Events::ENTITY::Fire);
        this->attackCD = 0;
    }
    else
    {
        this->attackCD += deltaTime;
    }
}
