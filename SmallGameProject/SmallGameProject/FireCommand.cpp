#include "FireCommand.h"

void FireCommand::execute(Actor& actor, float deltaTime)
{

    actor.Fire(deltaTime);

}
