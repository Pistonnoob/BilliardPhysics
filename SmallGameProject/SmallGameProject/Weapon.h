#ifndef WEAPON_H
#define WEAPON_H

#include "Model.h"
#include "Modifiers.h"
#include <string>
#include "Entity.h"


class Weapon {

private:
	std::string name;

	float attackDamage;
	float playerSpeed;
	float attackSpeed;

	Model* weaponModel;
public:
	Weapon();
	Weapon(const float &attackDamage, const float &playerSpeed, const float &attackSpeed);

	~Weapon();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string objFilename);
	virtual void ShutDown();

	Model* GetModel();


	float GetAttackDamageMod() const;
	float GetAttackSpeedMod() const;
	float GetPlayerSpeedMod() const;

	void ShootWeapon(Entity* entity, Events::UNIQUE_FIRE power);
	void ShootWeapon(Entity* entity);
};
#endif