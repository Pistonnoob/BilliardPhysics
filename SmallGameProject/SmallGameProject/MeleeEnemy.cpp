#include "MeleeEnemy.h"

MeleeEnemy::MeleeEnemy(float posX, float posZ)
{
    this->posX = posX;
    this->posZ = posZ;
    this->myType = Type::MELEEE;
	this->health = 150;
	this->damage = 10;
}

MeleeEnemy::~MeleeEnemy()
{
}

bool MeleeEnemy::Initialize(Model * model, EntitySubject* entitySubject, bool isSphere)
{
    this->aimDir = DirectX::XMFLOAT3(0, 0, 0);
	bool result = Entity::Initialize(model, entitySubject, isSphere);
	this->entitySubject->Notify(this, Events::ENTITY::MELEE_CREATED);
	return result;
}

void MeleeEnemy::Shutdown()
{
 	this->entitySubject->Notify(this, Events::ENTITY::MELEE_DEAD);
	Entity::Shutdown(true);
}

void MeleeEnemy::Fire(float deltaTime)
{
	if (this->attackCD > M_ATTACK_DELAY)
	{
		this->entitySubject->Notify(this, Events::UNIQUE_FIRE::ARCFIRE, 3.14 / 2, 5);
		this->attackCD = 0;
	}
	else
	{
		this->attackCD += deltaTime;
	}
}
