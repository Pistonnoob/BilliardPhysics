#ifndef ACTOR_H
#define ACTOR_H

#include "Entity.h"

class Actor : public Entity {

protected:
    DirectX::XMFLOAT3 aimDir;

public:
    Actor();
    virtual~Actor();


    virtual void MoveRight(float deltaTime) = 0;
    virtual void MoveLeft(float deltaTime) = 0;
    virtual void MoveUp(float deltaTime) = 0;
    virtual void MoveDown(float deltaTime) = 0;
	virtual void Fire(float deltaTime) = 0;
    virtual void Move(DirectX::XMFLOAT3 moveVec) = 0;

    virtual void SetAimDir(DirectX::XMFLOAT3 newDir);
    virtual DirectX::XMFLOAT3 GetAimDir();

};

#endif