#include "Abilities.h"

ReverseFire::ReverseFire() : Ability()
{
}
ReverseFire::~ReverseFire()
{
}
void ReverseFire::Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay)
{
	Ability::Initialize(arc, nrOfProjectiles, cooldown, attackDelay, maxCharges, triggerDelay);

}
float ReverseFire::Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos)
{
	if (this->cdCounter >= this->cooldown || this->chargesLeft > 0)
	{
		this->isActivated = true;

		float x = (playerPos.x - enemy->GetPosition().x) * 0.01f;
		float z = (playerPos.z - enemy->GetPosition().z) * 0.01f;

		enemy->SetAimDir(DirectX::XMFLOAT3(x, 0, z));


		entitySubject->Notify(enemy, Events::UNIQUE_FIRE::REVERSERBULLETS, this->arc, this->nrOfProjectiles, this->triggerDelay);


		this->cdCounter = 0;
		this->chargesLeft--;

		return this->attackDelay;
	}
	return -1;
}
void ReverseFire::Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime)
{
	Ability::Update(enemy, entitySubject, deltaTime);

}