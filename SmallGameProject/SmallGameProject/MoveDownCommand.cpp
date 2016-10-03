#include"MoveDownCommand.h"

void MoveDownCommand::execute(Actor& actor, float deltaTime)
{
    actor.MoveDown(deltaTime);
}