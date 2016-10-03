#ifndef MOVELEFTCOMMAND_H
#define MOVELEFTCOMMAND_H

#include "Command.h"

class MoveLeftCommand : public Command {

private:

public:
    virtual void execute(Actor& actor, float deltaTime);

};

#endif