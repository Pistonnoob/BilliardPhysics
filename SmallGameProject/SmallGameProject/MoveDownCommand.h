#ifndef MOVEDOWNCOMMAND_H
#define MOVEDOWNCOMMAND_H

#include "Command.h"

class MoveDownCommand : public Command {

private:

public:
    virtual void execute(Actor& actor, float deltaTime);

};

#endif