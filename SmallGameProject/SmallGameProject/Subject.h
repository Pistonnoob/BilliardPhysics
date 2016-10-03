#ifndef SUBJECT_H
#define SUBJECT_H

#include <set>
#include "Observer.h"

class Subject {

protected:
	std::set<Observer*> observers;
public:
	Subject();
	virtual ~Subject();
	virtual void AddObserver(Observer* observer) = 0;
	virtual void RemoveObserver(Observer* observer) = 0;
    virtual void Notify(Entity* entity, Events::ENTITY evnt) = 0;
	virtual void Notify(Entity* entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets) = 0;
	virtual void Notify(Entity* entity, Events::PICKUP evnt) = 0;
};

#endif