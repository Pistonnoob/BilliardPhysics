#include "Ai.h"

#include <ctime>
#include <cstdlib>
Ai::Ai()
{
    srand(unsigned(time(0)));
    this->nrOfActions = 2;
}
Ai::~Ai()
{

}
void Ai::updateActors(std::vector<Enemy*>& actors, DirectX::XMFLOAT3 playerPos, float deltaTime)
{
    for (int i = 0; i < actors.size(); i++)
    {
        updateActor(actors.at(i), playerPos, deltaTime);
    }
    separateActors(actors);
}
void Ai::updateActor(Enemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime)
{
	DirectX::XMMATRIX modelWorldMatrix;

	if (actor->GetType() == Type::BOMBER)
	{
		BomberEnemy* bPtr = (BomberEnemy*)actor;
		updateBomber(bPtr, playerPos, deltaTime);
	}
	else if (actor->GetType() == Type::RANGED)
	{
		RangedEnemy* bPtr = (RangedEnemy*)actor;
		updateRange(bPtr, playerPos, deltaTime);
	}
	else if(actor->GetType() == Type::MELEEE)
	{
		MeleeEnemy* bPtr = (MeleeEnemy*)actor;
		updateMelee(bPtr, playerPos, deltaTime);
	}
	else
	{
		Boss* bossPtr = (Boss*)actor;
		updateBoss(bossPtr, playerPos, deltaTime);
	}
    this->updateActorAimDir(actor, playerPos);
    for (int i = 0; i < this->nrOfActions; i++)
    {
        if (commands.size() != 0)
        {
            int action = rand() % commands.size();

            this->commands.at(action)->execute(*actor, deltaTime);
            delete this->commands.at(action);
            this->commands.at(action) = nullptr;
            this->commands.erase(this->commands.begin() + action);

        }
    }
	DirectX::XMFLOAT3 pos = actor->GetPosition();

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	actor->GetBV()->UpdateBoundingVolume(worldMatrix);

    for (int i = 0; i < commands.size(); i++)
    {
        if (this->commands.at(i) != nullptr)
            delete this->commands.at(i);
    }
    this->commands.clear();
}
void Ai::updateBomber(BomberEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime)
{

	float distance = distanceBetween(actor->GetPosition(), playerPos);
    if (distance > 7.5f)
    {
        moveToPlayer(actor, playerPos);
    }
	if (distance < 250.0f)
	{
		bool canExlpode = actor->ChargeExplosion(deltaTime);
		if (canExlpode == true)
		{

			this->commands.push_back(new FireCommand());
		}
	}

}
void Ai::updateRange(RangedEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime)
{
    float distance = distanceBetween(actor->GetPosition(), playerPos);
    if (distance > RANGED_MAX_DESIRED_DISTANCE)
    {
        moveToPlayer(actor, playerPos);
    }
    else if (distance < RANGED_MIN_DESIRED_DISTANCE)
    {
        moveAwayFromPlayer(actor, playerPos);
    }
    if(distance < RANGED_MAX_DESIRED_DISTANCE)
    {

        this->commands.push_back(new FireCommand());
    }
}
void Ai::updateMelee(MeleeEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime)
{
	float distance = distanceBetween(actor->GetPosition(), playerPos);
    if (distance > MELEE_MAX_DESIRED_DISTANCE)
    {
        moveToPlayer(actor, playerPos);
    }
    else if (distance < MELEE_MIN_DESIRED_DISTANCE)
    {
        moveAwayFromPlayer(actor, playerPos);
    }
	else
	{
		this->commands.push_back(new FireCommand());
	}
}
void Ai::updateBoss(Boss* actor, DirectX::XMFLOAT3 playerPos, float deltaTime)
{
	actor->update(playerPos, deltaTime);
	float distance = distanceBetween(actor->GetPosition(), playerPos);
	if (distance > 100.0f)
	{
		this->moveToPlayer(actor, playerPos);
	}
	if (distance < 60.0f)
	{
		this->moveAwayFromPlayer(actor, playerPos);
	}
}

//private functions
void Ai::moveToPlayer(Enemy* actor, DirectX::XMFLOAT3 playerPos)
{
    DirectX::XMFLOAT3 pos = actor->GetPosition();
    if (pos.x < playerPos.x && actor->GetPosition().x < BORDER_RIGHT)
    {
        this->commands.push_back(new MoveRightCommand());
    }
    else if (pos.x > playerPos.x && actor->GetPosition().x > BORDER_LEFT)
    {
        this->commands.push_back(new MoveLeftCommand());
    }
    if (pos.z < playerPos.z && actor->GetPosition().z < BORDER_UP)
    {
        this->commands.push_back(new MoveUpCommand());
    }
    else if (pos.z > playerPos.z && actor->GetPosition().z > BORDER_DOWN)
    {
        this->commands.push_back(new MoveDownCommand());
    }
}
void Ai::moveAwayFromPlayer(Enemy* actor, DirectX::XMFLOAT3 playerPos)
{
    DirectX::XMFLOAT3 pos = actor->GetPosition();
    if (pos.x < playerPos.x && actor->GetPosition().x > BORDER_LEFT)
    {
        this->commands.push_back(new MoveLeftCommand());
    }
    else if (pos.x > playerPos.x && actor->GetPosition().x < BORDER_RIGHT)
    {
        this->commands.push_back(new MoveRightCommand());
    }
    if (pos.z < playerPos.z && actor->GetPosition().z > BORDER_DOWN)
    {
        this->commands.push_back(new MoveDownCommand());
    }
    else if (pos.z > playerPos.z && actor->GetPosition().z < BORDER_UP)
    {
        this->commands.push_back(new MoveUpCommand());
    }
}
void Ai::separateActors(std::vector<Enemy*>& actors)
{
    for (int i = 0; i < actors.size(); i++)
    {
        for (int a = 1; a < actors.size() && a != i; a++)
        {
            DirectX::XMFLOAT3 pos1 = actors.at(i)->GetPosition();
            DirectX::XMFLOAT3 pos2 = actors.at(a)->GetPosition();

            float d = distanceBetween(pos1, pos2);
            if (d <= 0.001f)
            {
                DirectX::XMFLOAT3 temp;
                temp.x = (rand() % 10);
                temp.y = 0.0f;
                temp.z = (rand() % 10);

                seperate(actors.at(i), actors.at(a), 0.05f, temp);
            }
            if (d < 5)
            {
                DirectX::XMFLOAT3 temp;
                temp.x = pos1.x - pos2.x;
                temp.y = 0.0f;
                temp.z = pos1.z - pos2.z;

                seperate(actors.at(i), actors.at(a), 0.05f, temp);
            }
            if (d < 7 && actors.at(i)->GetType() != actors.at(a)->GetType())
            {
                DirectX::XMFLOAT3 temp;
                temp.x = pos1.x - pos2.x;
                temp.y = 0.0f;
                temp.z = pos1.z - pos2.z;

                seperate(actors.at(i), actors.at(a), 0.01f, temp);
            }
        }
    }
}

float Ai::distanceBetween(DirectX::XMFLOAT3 positon1, DirectX::XMFLOAT3 positon2)
{

    float xDiff = abs(positon1.x - positon2.x);
    float yDiff = abs(positon1.y - positon2.y);
    float zDiff = abs(positon1.z - positon2.z);

    return xDiff + yDiff + zDiff;
    //return pow(xDiff + yDiff + zDiff, 2);
}    
void Ai::seperate(Enemy* actor1, Enemy* actor2, float amplitude, DirectX::XMFLOAT3 dir)
{
    dir.x *= amplitude;
    dir.z *= amplitude;
    actor1->Move(dir);

    dir.x *= -1.0f;
    dir.z *= -1.0f;
    actor2->Move(dir);
}

void Ai::updateActorAimDir(Enemy * actor, DirectX::XMFLOAT3 playerPos)
{
    float x = (playerPos.x - actor->GetPosition().x);
    float z = (playerPos.z - actor->GetPosition().z);

    DirectX::XMVECTOR vec = DirectX::XMVectorSet(x, 0, z, 0);
    vec = DirectX::XMVector4Normalize(vec);

    x = DirectX::XMVectorGetX(vec);
    z = DirectX::XMVectorGetZ(vec);

    actor->SetAimDir(DirectX::XMFLOAT3(x, 0, z));
}

int Ai::getNrOfActions() const
{
    return this->nrOfActions;
}
void Ai::setNrOfActions(int nrOfActions)
{
    this->nrOfActions = nrOfActions;
}