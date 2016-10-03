#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include "Observer.h"
#include "SoundHandler.h"

class AudioHandler : public Observer {

private:
	SoundHandler* soundH;

public:
	AudioHandler();
	~AudioHandler();

	bool Initialize(HWND hwnd);
	bool ShutDown();

	void PlayAudio(char* fileName);

	virtual void OnNotify(Entity* entity, Events::ENTITY evnt);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit);
	virtual void OnNotify(Entity* entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets);
	virtual void OnNotify(Entity* entity, Events::PICKUP evnt);

};

#endif