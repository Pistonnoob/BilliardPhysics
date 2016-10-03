#include "Abilities.h"
#include "Algorithm.h"

ArcFire::ArcFire() : Ability()
{
}
ArcFire::~ArcFire()
{
}
void ArcFire::Initialize(float arc, int nrOfProjectiles, float cooldown, int attackDelay, int maxCharges, float triggerDelay)
{
	Ability::Initialize(arc, nrOfProjectiles, cooldown, attackDelay, maxCharges, triggerDelay);

}
float ArcFire::Activate(Enemy* enemy, EntitySubject* entitySubject, DirectX::XMFLOAT3 playerPos)
{
	if (this->cdCounter >= this->cooldown || this->chargesLeft > 0)
	{
		float x = (playerPos.x - enemy->GetPosition().x);
		float z = (playerPos.z - enemy->GetPosition().z);

		enemy->SetAimDir(DirectX::XMFLOAT3(x, 0, z));

		entitySubject->Notify(enemy, Events::UNIQUE_FIRE::ARCFIRE, this->arc, this->nrOfProjectiles);

		this->cdCounter = 0;
		this->chargesLeft--;

		return this->attackDelay;

	}
	return -1;

}
void ArcFire::Update(Enemy* enemy, EntitySubject* entitySubject, float deltaTime)
{
	Ability::Update(enemy, entitySubject, deltaTime);

}