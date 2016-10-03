#include "Projectile.h"

Projectile::Projectile(unsigned int damage) : Entity()
{
    this->posX = 0;
    this->posZ = 0;
    this->origin = DirectX::XMFLOAT3(0, 0, 0);
    this->moveDir = DirectX::XMFLOAT3(0, 0, 0);
	this->damage = damage;
}
Projectile::~Projectile()
{

}
void Projectile::Initialize(Model* model, EntitySubject* entitySubject, float posX, float posZ, DirectX::XMFLOAT3 moveDir)
{
    this->posX = posX;
    this->posZ = posZ;
    this->moveDir = moveDir;
    this->origin = DirectX::XMFLOAT3(posX, 0, posZ);
    Entity::Initialize(model, entitySubject, true);
}
void Projectile::Shutdown()
{
    Entity::Shutdown(true);
}
void Projectile::update(float deltaTime)
{
    this->posX += (this->moveDir.x * 100) * deltaTime;
    this->posZ += (this->moveDir.z * 100) * deltaTime;
}
void Projectile::setMoveDir(DirectX::XMFLOAT3 newMoveDir)
{
    this->moveDir = newMoveDir;
}
void Projectile::setPos(DirectX::XMFLOAT3 newPos)
{
    this->posX = newPos.x;
    this->posZ = newPos.z;
}
DirectX::XMFLOAT3 Projectile::getOrigin()
{
    return this->origin;
}
DirectX::XMFLOAT3 Projectile::getMoveDir()
{
    return this->moveDir;
}
DirectX::XMFLOAT3 Projectile::getPos()
{
    DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(this->posX, 0, this->posZ);
    return pos;
}

unsigned int Projectile::GetDamage()
{
	return this->damage;
}
