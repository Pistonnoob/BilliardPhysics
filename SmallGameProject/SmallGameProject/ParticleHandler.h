#ifndef PARTICLEHANDLER_H
#define PARTICLEHANDLER_H
#include "Observer.h"
#include "Algorithm.h"
#include "Texture.h"
#include "EmitterPrototype.h"
#include "EmitterPlayerSpawn.h"
#include "EmitterEnemySpawn.h"
#include "EmitterExplosion.h"
#include "EmitterClusterExplosion.h"
#include "EmitterSpawnPulse.h"
#include "GraphicHandler.h"
#include "CameraHandler.h"

class ParticleHandler :
	public Observer
{
private:
	std::vector<ParticleEmitter*> emitters;
	EmitterSpawnPulse spawnEmitter;

	Texture myTextures;
	ID3D11Device* device;

	struct Emitter_Removal_Predicate {
		bool operator()(ParticleEmitter* emitter) {
			bool result = false;
			if (emitter)
			{
				if (emitter->IsCompleted())
				{
					emitter->Shutdown();
					delete emitter;
					emitter = nullptr;
					result = true;
				}
			}
			else
				result = true;
			return result;
		}
	};

public:
	ParticleHandler();
	virtual ~ParticleHandler();
	void Shutdown();

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	virtual void OnNotify(Entity* entity, Events::ENTITY evnt);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets);
	virtual void OnNotify(Entity* entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay);
	virtual void OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit);
	virtual void OnNotify(Entity* entity, Events::PICKUP evnt);

	int Update(float dT, ID3D11DeviceContext* deviceContext);

	int Render(GraphicHandler* gHandler, CameraHandler* camera);
private:	//Functions
	void KillEmitters();
};

#endif