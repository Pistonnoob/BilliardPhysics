#ifndef AI_H
#define AI_H
#include "Command.h"
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "MoveDownCommand.h"
#include "MoveUpCommand.h"
#include "FireCommand.h"
#include "BomberEnemy.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "Boss.h"
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

static int RANGED_MIN_DESIRED_DISTANCE = 50;
static int RANGED_MAX_DESIRED_DISTANCE = 100;

static int MELEE_MIN_DESIRED_DISTANCE = 15;
static int MELEE_MAX_DESIRED_DISTANCE = 20;

static float BORDER_LEFT = -44.0f;
static float BORDER_RIGHT = 44.0f;
static float BORDER_UP = 44.0f;
static float BORDER_DOWN = -44.0f;

class Ai {

private:
    std::vector<Command*> commands;
    int nrOfActions;
    DirectX::XMFLOAT3 playerPos;

    void moveToPlayer(Enemy* actor, DirectX::XMFLOAT3 playerPos);
    void moveAwayFromPlayer(Enemy* actor, DirectX::XMFLOAT3 playerPos);
    float distanceBetween(DirectX::XMFLOAT3 positon1, DirectX::XMFLOAT3 positon2);
    void seperate(Enemy* actor1, Enemy* actor2, float amplitude, DirectX::XMFLOAT3 dir);

    void updateActorAimDir(Enemy* actor, DirectX::XMFLOAT3 playerPos);
public:
    Ai();
    ~Ai();

    void updateActors(std::vector<Enemy*>& actors, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void updateActor(Enemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void updateBomber(BomberEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void updateRange(RangedEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void updateMelee(MeleeEnemy* actor, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void updateBoss(Boss* actor, DirectX::XMFLOAT3 playerPos, float deltaTime);
    void separateActors(std::vector<Enemy*>& actors);

    int getNrOfActions() const;
    void setNrOfActions(int nrOfActions);

};

#endif