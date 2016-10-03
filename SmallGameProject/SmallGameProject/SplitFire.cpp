#include "Abilities.h"

SplitFire::SplitFire() : Ability()
{
}
SplitFire::~SplitFire()
{
}
void SplitFire::Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay, int projectilesOnSplit, float splitArc)
{
	Ability::Initialize(arc, nrOfProjectiles, cooldown, attackDelay, maxCharges, triggerDelay);
	this->projectilesOnSplit = projectilesOnSplit;
	this->splitArc = splitArc;

}
float SplitFire::Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos)
{
	if (this->cdCounter >= this->cooldown || this->chargesLeft > 0)
	{
		this->isActivated = true;

		float x = (playerPos.x - enemy->GetPosition().x);
		float z = (playerPos.z - enemy->GetPosition().z);

		enemy->SetAimDir(DirectX::XMFLOAT3(x, 0, z));

		entitySubject->Notify(enemy, Events::UNIQUE_FIRE::SPLITFIRE, this->arc, this->nrOfProjectiles, this->triggerDelay, this->splitArc, this->projectilesOnSplit);


		this->cdCounter = 0;
		this->chargesLeft--;

		return this->attackDelay;
	}
	return -1;

}
void SplitFire::Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime)
{
	Ability::Update(enemy, entitySubject, deltaTime);

}