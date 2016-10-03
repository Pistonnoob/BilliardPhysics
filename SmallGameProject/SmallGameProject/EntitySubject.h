#ifndef ENTITYSUBJECT_H
#define ENTITYSUBJECT_H

#include "Subject.h"

class EntitySubject : public Subject{

	private:
		std::set<Observer*> observers;

	public:
		EntitySubject();
		virtual ~EntitySubject();
        void ShutDown();
		virtual void AddObserver(Observer* observer);
		virtual void RemoveObserver(Observer* observer);
        void Notify(Entity* entity, Events::ENTITY evnt);
        void Notify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets);
		void Notify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay);
		void Notify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit);
		void Notify(Entity* entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets);
		void EntitySubject::Notify(Entity* entity, Events::PICKUP evnt);


};

#endif