#include "AudioHandler.h"

AudioHandler::AudioHandler()
{
	this->soundH = nullptr;
}

AudioHandler::~AudioHandler()
{
}

bool AudioHandler::Initialize(HWND hwnd)
{
	if (this->soundH == nullptr) {
		
		this->soundH = new SoundHandler();
		if (!this->soundH->Initialize(hwnd)) {
			MessageBox(hwnd, L"Could not initialize Direct Sound.", L"Error", MB_OK);
			return false;
		}

	}
	
	return true;
}

bool AudioHandler::ShutDown()
{
	if (this->soundH) {
		this->soundH->Shutdown();
		delete this->soundH;
		this->soundH = nullptr;
	}
	
	return true;
}

void AudioHandler::PlayAudio(char* fileName)
{
	if (this->soundH) {
		this->soundH->PlayWaveFile(fileName);
	}
}

void AudioHandler::OnNotify(Entity * entity, Events::ENTITY evnt)
{
	if (evnt == Events::ENTITY::Fire) {
		this->soundH->PlayWaveFile("gun");
	}
	else if (evnt == Events::ENTITY::MELEE_DEAD
		|| Events::ENTITY::BOMBER_DEAD
		|| Events::ENTITY::RANGED_DEAD) 
	{
		this->soundH->PlayWaveFile("explosion");
	}

}

void AudioHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets)
{
}

void AudioHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay)
{
}

void AudioHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit)
{
}

void AudioHandler::OnNotify(Entity * entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets)
{
}

void AudioHandler::OnNotify(Entity * entity, Events::PICKUP evnt)
{
}
