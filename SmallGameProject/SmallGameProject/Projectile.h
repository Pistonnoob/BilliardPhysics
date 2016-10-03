#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Entity.h"

class Projectile : public Entity {

private:
	unsigned int damage;
    DirectX::XMFLOAT3 origin;
    DirectX::XMFLOAT3 moveDir;

public:
    Projectile(unsigned int damage);
    void Initialize(Model* model,EntitySubject* entitySubject, float posX, float posZ, DirectX::XMFLOAT3 moveDir);
    void Shutdown();
    virtual ~Projectile();
    void update(float deltaTime);
    void setMoveDir(DirectX::XMFLOAT3 newMoveDir);
    void setPos(DirectX::XMFLOAT3 newPos);
    DirectX::XMFLOAT3 getOrigin();
    DirectX::XMFLOAT3 getMoveDir();
    DirectX::XMFLOAT3 getPos();
	unsigned int GetDamage();

};

#endif