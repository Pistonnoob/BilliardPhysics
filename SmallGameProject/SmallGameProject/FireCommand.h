#ifndef FIRECOMMAND_H
#define FIRECOMMAND_H

#include "Command.h"

class FireCommand : public Command {

private:
    DirectX::XMFLOAT3 dir;
public:
    virtual void execute(Actor& actor, float deltaTime);

};

#endif
