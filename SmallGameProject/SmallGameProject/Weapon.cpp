#include "Weapon.h"

//not so strong weapon
Weapon::Weapon()
{

	this->weaponModel = new Model();

	this->attackDamage = 1.0f;
	this->playerSpeed = 1.0f;
	this->attackSpeed = 1.0f;
}

//if you want to change the base-Stats
Weapon::Weapon(const float &attackDamage, const float &playerSpeed, const float &attackSpeed)
{
	this->weaponModel = new Model();

	this->attackDamage = attackDamage;
	this->playerSpeed = playerSpeed;
	this->attackSpeed = attackSpeed;
}

Weapon::~Weapon()
{
}

bool Weapon::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string objFilename)
{
	bool result = true;

	result = this->weaponModel->Initialize(device, deviceContext, objFilename);

	//setting the starting values
	this->attackDamage = 10.0f;
	this->attackSpeed = 10.0f;
	this->playerSpeed = 10.0f;

	return result;
}

void Weapon::ShutDown()
{
	if (this->weaponModel) 
	{
		this->weaponModel->Shutdown();
		delete this->weaponModel;
		this->weaponModel = nullptr;
	}
}

Model * Weapon::GetModel()
{
	return this->weaponModel;
}

float Weapon::GetAttackDamageMod() const
{
	return this->attackDamage;
}

float Weapon::GetAttackSpeedMod() const
{
	return this->attackSpeed;
}

float Weapon::GetPlayerSpeedMod() const
{
	return this->playerSpeed;
}

void Weapon::ShootWeapon(Entity * entity, Events::UNIQUE_FIRE power)
{
	//player powerup

	switch(power)
	{
		case Events::UNIQUE_FIRE::ARCFIRE:
			entity->GetEntitySubject()->Notify(entity, Events::UNIQUE_FIRE::ARCFIRE, 3.14 / 2, 3);
			break;
		case Events::UNIQUE_FIRE::SPLITFIRE:
			entity->GetEntitySubject()->Notify(entity, Events::UNIQUE_FIRE::SPLITFIRE, 3.14 / 4, 1, 1.0f / 3.0f, 3.14 / 2, 5);
			break;
		case Events::UNIQUE_FIRE::REVERSERBULLETS:
			entity->GetEntitySubject()->Notify(entity, Events::UNIQUE_FIRE::REVERSERBULLETS, 3.14 / 4, 10, 1);
			break;
	}	
}

void Weapon::ShootWeapon(Entity * entity)
{
	entity->GetEntitySubject()->Notify(entity, Events::ENTITY::Fire);
}
