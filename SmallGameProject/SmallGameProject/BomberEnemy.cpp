#include "BomberEnemy.h"

BomberEnemy::BomberEnemy(float posX, float posZ)
{
    this->posX = posX;
    this->posZ = posZ;
    this->myType = Type::BOMBER;
	this->health = 100;
	this->damage = 50;
}

BomberEnemy::~BomberEnemy()
{
}

bool BomberEnemy::Initialize(Model* model, EntitySubject* entitySubject, bool isSphere)
{
    this->aimDir = DirectX::XMFLOAT3(0, 0, 0);
	bool result = Entity::Initialize(model, entitySubject, isSphere);
	this->entitySubject->Notify(this, Events::ENTITY::BOMBER_CREATED);
	return result;
}

void BomberEnemy::Shutdown()
{
	this->entitySubject->Notify(this, Events::ENTITY::BOMBER_DEAD);
	Entity::Shutdown(true);
}

bool BomberEnemy::ChargeExplosion(float deltaTime)
{
	bool canExplode = false;
	if (this->attackCD > -1)
	{
		this->attackCD += deltaTime;
	}
	if (this->attackCD > EXPLOSION_DELAY)
	{
		canExplode = true;
	}
	return canExplode;
}

void BomberEnemy::Fire(float deltaTime)
{
	if (this->attackCD > EXPLOSION_DELAY)
	{
		//this->entitySubject->Notify(this, Events::UNIQUE_FIRE::ARCFIRE, 3.14f * 2, 20);
		this->entitySubject->Notify(this, Events::ENTITY::BOMBER_DEAD);

		this->entitySubject->Notify(this, Events::UNIQUE_FIRE::ARCFIRE, 3.14f * 2, 20);
        this->health = 0;	//set health to 0
		this->attackCD = -1;
	}
	else if(this->attackCD > -1)
	{
		this->attackCD += deltaTime;
	}
}



