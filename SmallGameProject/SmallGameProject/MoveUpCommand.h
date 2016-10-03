#ifndef MOVEUPCOMMAND_H
#define MOVEUPCOMMAND_H

#include "Command.h"

class MoveUpCommand : public Command {

private:

public:
    virtual void execute(Actor& actor, float deltaTime);

};

#endif