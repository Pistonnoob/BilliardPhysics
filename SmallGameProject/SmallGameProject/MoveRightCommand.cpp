#include "MoveRightCommand.h"


void MoveRightCommand::execute(Actor& actor, float deltaTime)
{
    actor.MoveRight(deltaTime);
}