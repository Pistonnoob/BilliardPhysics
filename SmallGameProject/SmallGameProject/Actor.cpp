#include "Actor.h"

Actor::Actor() : Entity()
{

}

Actor::~Actor()
{
}
void Actor::SetAimDir(DirectX::XMFLOAT3 newDir)
{
    this->aimDir = newDir;
}
DirectX::XMFLOAT3 Actor::GetAimDir()
{
    return this->aimDir;
}