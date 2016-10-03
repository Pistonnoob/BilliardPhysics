#ifndef OBSERVER_H
#define OBSERVER_H

#include "EventLib.h"
class Entity;

class Observer {

private:

public:
	Observer();
	virtual ~Observer();
	virtual void OnNotify(Entity* entity, Events::ENTITY evnt) = 0;
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets) = 0;
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay) = 0;
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit) = 0;
	virtual void OnNotify(Entity* entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets) = 0;
	virtual void OnNotify(Entity* entity, Events::PICKUP evnt) = 0;

};

#endif