#ifndef MOVERIGHTCOMMAND_H
#define MOVERIGHTCOMMAND_H

#include "Command.h"

class MoveRightCommand : public Command {

private:

public:
    virtual void execute(Actor& actor, float deltaTime);

};

#endif