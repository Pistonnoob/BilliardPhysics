#include "MoveLeftCommand.h"


void MoveLeftCommand::execute(Actor& actor, float deltaTime)
{
    actor.MoveLeft(deltaTime);
}