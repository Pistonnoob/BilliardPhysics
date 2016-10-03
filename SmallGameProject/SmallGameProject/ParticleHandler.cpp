#include "ParticleHandler.h"
#include "Player.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "BomberEnemy.h"


ParticleHandler::ParticleHandler()
{
	this->device = nullptr;
}


ParticleHandler::~ParticleHandler()
{
}

void ParticleHandler::Shutdown()
{
	this->myTextures.Shutdown();

	while (this->emitters.size())
	{
		ParticleEmitter* emitter = this->emitters.back();
		emitter->Shutdown();
		delete emitter;
		emitter = nullptr;
		this->emitters.pop_back();
	}
	this->emitters.clear();
	this->spawnEmitter.Shutdown();
}

void ParticleHandler::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	this->myTextures = Texture();
	std::string theNightSky = "Particles.mtl";
	bool victory = this->myTextures.Initialize(device, deviceContext, theNightSky);
	this->device = device;
	EmitterClusterExplosion* newEmitter = new EmitterClusterExplosion();
	newEmitter->Initialize(device, this->myTextures.GetTexture(0), 4.0f, 0.1f, 100);
	this->emitters.push_back(newEmitter);
	this->spawnEmitter = EmitterSpawnPulse();
	this->spawnEmitter.Initialize(device, this->myTextures.GetTexture(0), 1000.0f);
}

void ParticleHandler::OnNotify(Entity * entity, Events::ENTITY evnt)
{
	ParticleEmitter* newEmitter = nullptr;
	switch (evnt)
	{
	case Events::CREATED:
		break;
	case Events::PLAYER_CREATED:
	{
		/*newEmitter = new EmitterPlayerSpawn();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 4.0f);
		newEmitter->ApplyPosition(entity->GetPosition());*/
		//DirectX::XMFLOAT3 entityPosition = entity->GetPosition();
		//this->spawnEmitter.AddSpawnPulse(entityPosition.x, entityPosition.y, entityPosition.z, 0.2f, 4.0f, 0.1f, 0.8f, 0.8f, 3.0f);
	}
		break;
	case Events::PLAYER_MOVING:
		break;
	case Events::PLAYER_DEAD:
		break;
    case Events::PLAYER_HIT:
    {
        newEmitter = new EmitterClusterExplosion();
        EmitterClusterExplosion* temp = (EmitterClusterExplosion*)newEmitter;
        temp->Initialize(device, this->myTextures.GetTexture(0), 0.01f, 0.001f, 1);
        temp->ApplyPosition(entity->GetPosition());
    }
        break;
	case Events::BOMBER_CREATED:
	{
		newEmitter = new EmitterEnemySpawn();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 3.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
	}
		break;
	case Events::BOMBER_MOVING:
		break;
	case Events::BOMBER_DEAD:
	{
		newEmitter = new EmitterClusterExplosion();
        EmitterClusterExplosion* temp = (EmitterClusterExplosion*)newEmitter;
        temp->Initialize(device, this->myTextures.GetTexture(0), 4.0f, 0.1f, 100);
        temp->ApplyPosition(entity->GetPosition());
	}
		break;
	case Events::RANGED_CREATED:
	{
		newEmitter = new EmitterEnemySpawn();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 3.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
	}
		break;
	case Events::RANGED_MOVING:
		break;
	case Events::RANGED_DEAD:
		newEmitter = new EmitterExplosion();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 1.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
		break;
	case Events::MELEE_CREATED:
	{
		newEmitter = new EmitterEnemySpawn();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 3.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
	}
		break;
	case Events::MELEE_MOVING:
		break;
	case Events::MELEE_DEAD:
		newEmitter = new EmitterExplosion();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 1.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
		break;
	case Events::IDLE:
		break;
	case Events::MOVING:
		break;
	case Events::DEAD:
		newEmitter = new EmitterExplosion();
		newEmitter->Initialize(this->device, this->myTextures.GetTexture(0), 1.0f);
		newEmitter->ApplyPosition(entity->GetPosition());
		break;
	default:
		break;
	}
	if (newEmitter)
	{
		this->emitters.push_back(newEmitter);
	}
}

void ParticleHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets)
{
}

void ParticleHandler::OnNotify(Entity * entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets)
{
}

void ParticleHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay)
{
}

void ParticleHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit)
{
}

void ParticleHandler::OnNotify(Entity * entity, Events::PICKUP evnt)
{
}

int ParticleHandler::Update(float dT, ID3D11DeviceContext * deviceContext)
{
	int result = 0;

	for (auto emitter = this->emitters.begin(); emitter != this->emitters.end(); emitter++)
	{
 		(*emitter)->Update(dT, deviceContext);
	}
	this->spawnEmitter.Update(dT, deviceContext);
	this->KillEmitters();
	return result;
}

int ParticleHandler::Render(GraphicHandler * gHandler, CameraHandler * camera)
{
	int result = 0;

	ParticleShaderParameters parameters;
	parameters.diffTexture = nullptr;
	DirectX::XMFLOAT4 cameraPosition = camera->GetCameraPos();
	int amountOfParticles = 0;
	for(auto emitter : this->emitters)
	{
		emitter->SetCameraPos(cameraPosition);
		emitter->SortParticles();
		emitter->Render(gHandler->GetDeviceContext(), parameters, amountOfParticles);
		if(parameters.diffTexture == nullptr)
			parameters.diffTexture = this->myTextures.GetTexture(0);
		emitter->GetWorld(parameters.worldMatrix);
		if (!parameters.diffTexture)
		{
			result = false;
		}

		gHandler->ParticleRender(&parameters, camera, amountOfParticles);
	}
	this->spawnEmitter.SetCameraPos(cameraPosition);
	this->spawnEmitter.Render(gHandler->GetDeviceContext(), parameters, amountOfParticles);
	if (!parameters.diffTexture)
	{
		result = false;
	}
	gHandler->ParticleRender(&parameters, camera, amountOfParticles);
	return result;
}

void ParticleHandler::KillEmitters()
{
	this->emitters.erase(std::remove_if(this->emitters.begin(), this->emitters.end(), Emitter_Removal_Predicate()), this->emitters.end());
}
