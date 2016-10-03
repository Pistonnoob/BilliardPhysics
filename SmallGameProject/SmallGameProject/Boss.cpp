#include "Boss.h"

Boss::Boss(float posX, float posZ)
{
	this->posX = posX;
	this->posZ = posZ;
	this->abilityUsed = 0;
    this->myType = Type::BOSS;
    this->health = 500;
    this->damage = 10;
}
Boss::~Boss()
{

}

bool Boss::Initialize(Model * model, EntitySubject * entitySubject, bool isSphere)
{
	this->aimDir = DirectX::XMFLOAT3(0, 0, 0);
	return Entity::Initialize(model, entitySubject, isSphere);
}

bool Boss::Initialize(Model* model, EntitySubject* entitySubject, bool isSphere, int nrOfAbilities)
{
	this->aimDir = DirectX::XMFLOAT3(0, 0, 0);

	ArcFire* temp1 = new ArcFire();
	float arc			= 0.0f;
	int nrOfProjectiles	= 1;
	float cooldown		= 0.5f;
	float attackDelay	= 0.5f;
	int charges			= 1;
	float triggerDelay	= 1;
	temp1->Initialize(arc, nrOfProjectiles, cooldown, attackDelay, charges, triggerDelay);
    //delete temp1;
	this->abilities.push_back(temp1);

	SplitFire* temp2 = new SplitFire();
	arc						= 3.14f / 2;
	nrOfProjectiles			= 5;
	cooldown				= 4;
	attackDelay				= 1;
	charges					= 1;
	triggerDelay			= 0.5f;
	int projeciltesOnSplit	= 5;
	float splitArc			= 3.14f / 2;
	temp2->Initialize(arc, nrOfProjectiles, cooldown, attackDelay, charges, triggerDelay, projeciltesOnSplit, splitArc);
	this->abilities.push_back(temp2);

	ReverseFire* temp3 = new ReverseFire();
	arc					= 3.14f * 2;
	nrOfProjectiles		= 15;
	cooldown			= 8;
	attackDelay			= 3;
	charges				= 1;
	triggerDelay		= 2.5f;
	temp3->Initialize(arc, nrOfProjectiles, cooldown, attackDelay, charges, triggerDelay);
	this->abilities.push_back(temp3);

	return Entity::Initialize(model, entitySubject, isSphere);
}
bool Boss::Initialize(Model* model, EntitySubject* entitySubject, bool isSphere, std::vector<Ability*> abilities)
{
	this->aimDir = DirectX::XMFLOAT3(0, 0, 0);

	for (int i = 0; i < abilities.size(); i++)
	{
		this->abilities.push_back(abilities.at(i));
	}

	return Entity::Initialize(model, entitySubject, isSphere);

}
void Boss::Shutdown()
{
	for (int i = 0; i < this->abilities.size(); i++)
	{
		delete this->abilities.at(i);
	}
	this->abilities.clear();
	Entity::Shutdown(true);
}

void Boss::update(DirectX::XMFLOAT3 playerPos, float deltaTime)
{


	for (int i = 0; i < this->abilities.size(); i++)
	{
		this->abilities.at(i)->Update(this, this->GetEntitySubject(), deltaTime);

	}
	this->attackDelay -= deltaTime;
	int counter = 0;
	while (counter < this->abilities.size() && this->attackDelay <= 0)
	{
		float result = this->abilities.at(this->abilityUsed)->Activate(this, this->GetEntitySubject(), playerPos);
		this->abilityUsed++;
		if (result == -1)
		{
			counter++;
		}
		else
		{
			this->attackDelay = result;
		}
		if (this->abilityUsed >= this->abilities.size())
		{
			this->abilityUsed = 0;
		}
	}
    this->aimDir.x = playerPos.x - this->posX;
    this->aimDir.y = 0;
    this->aimDir.z = playerPos.z - this->posZ;

}
