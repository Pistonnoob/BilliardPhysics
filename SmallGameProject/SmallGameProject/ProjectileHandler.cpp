#include "ProjectileHandler.h"

ProjectileHandler::ProjectileHandler()
{
	this->m_ball = Model();
}
ProjectileHandler::~ProjectileHandler()
{

}
bool ProjectileHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, EntitySubject* subject)
{
	bool result = true;
    this->subject = subject;
	result = this->m_ball.Initialize(device, deviceContext, "projectile");
    //result = this->m_ball.Initialize(device, deviceContext, "box");

	return result;
}
void ProjectileHandler::ShutDown()
{
	this->m_ball.Shutdown();
	for (int i = 0; i < this->projectiles.size(); i++)
	{
		Projectile* temp = this->projectiles.at(i);

        temp->Shutdown();
        delete temp;

	}
	this->projectiles.clear();

}
void ProjectileHandler::Update(float deltaTime)
{
	for (int i = 0; i < this->projectiles.size(); i++)
	{
		Projectile* temp = this->projectiles.at(i);
		temp->update(deltaTime);

		DirectX::XMMATRIX modelWorldMatrix;

		//temp->GetModel()->GetWorldMatrix(modelWorldMatrix);
		DirectX::XMFLOAT3 pos = this->projectiles.at(i)->GetPosition();
		modelWorldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		this->projectiles.at(i)->GetModel()->SetWorldMatrix(modelWorldMatrix);
		
		temp->GetBV()->UpdateBoundingVolume(modelWorldMatrix);

		if (pos.x < -44.0f || pos.x > 44.0f || pos.z < -44.0f || pos.z > 44.0f)
		{
			//Projectile* temp = this->projectiles.at(i);
			temp->Shutdown();
			delete temp;
			//this->projectiles.at(i) = nullptr;
			this->projectiles.erase(projectiles.begin() + i);
            this->onProjectileRemoved(i);
			i--;

        }
	}
	for (int i = 0; i < this->eventsToTrack.size(); i++)
	{
		this->eventsToTrack.at(i).timer -= deltaTime;
		if (this->eventsToTrack.at(i).timer <= 0)
		{
			triggerEvent(this->eventsToTrack.at(i), 0, 0);
			this->eventsToTrack.erase(this->eventsToTrack.begin() + i);
			i--;
		}
	}
}
void ProjectileHandler::Render(GraphicHandler * gHandler, CameraHandler* camera)
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMMATRIX worldMatrix;
    for (auto projectile : this->projectiles)
    {
        pos = projectile->GetPosition();
        worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y + 0.5f, pos.z);

        DirectX::XMFLOAT3 dirVec = projectile->getMoveDir();
        float angle = atan2(dirVec.z, dirVec.x);

        DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationY(-angle);

        this->m_ball.SetWorldMatrix(rotMatrix * worldMatrix);
        gHandler->DeferredRender(&this->m_ball, camera);
    }

	for (auto projectile : this->projectiles)
	{
		pos = projectile->GetPosition();
		worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y + 0.5f, pos.z);
		this->m_ball.SetWorldMatrix(worldMatrix);
		gHandler->ShadowRender(&this->m_ball, camera);
	}

}
bool ProjectileHandler::IntersectionTest(Entity * entity)
{
	bool result = false;
	int i = -1;
	for (auto projectile : this->projectiles) {
		i++;
		if (entity->GetBV()->Intersect(projectile->GetBV())) {
			
			entity->ApplyDamage(projectile->GetDamage());
			
			this->projectiles.at(i)->Shutdown();
			delete projectile;
			this->projectiles.erase(this->projectiles.begin() + i);
            this->onProjectileRemoved(i);


			i--;
			return true;
		}
	}

	return false;
}
void ProjectileHandler::OnNotify(Entity* entity, Events::ENTITY evnt)
{

	switch (evnt)
	{
	case(Events::ENTITY::Fire) :

		DirectX::XMFLOAT3 dir = entity->GetAimDir();
		DirectX::XMFLOAT3 pos = entity->GetPosition();

		DirectX::XMVECTOR dirr = DirectX::XMVectorSet(dir.x, dir.y, dir.z, 0.0f);
		dirr = DirectX::XMVector3Normalize(dirr);

		float x = DirectX::XMVectorGetX(dirr);
		float y = DirectX::XMVectorGetY(dirr);
		float z = DirectX::XMVectorGetZ(dirr);

		dir.x = x;
		dir.y = y;
		dir.z = z;

		this->projectiles.push_back(new Projectile(entity->GetDamage()));
		this->projectiles.at(this->projectiles.size() - 1)->Initialize(&this->m_ball, nullptr, pos.x, pos.z, dir);
		break;
	}
}

void ProjectileHandler::OnNotify(Entity* entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets)
{
	trigger_event event;
	switch (evnt)
	{
	case(Events::UNIQUE_FIRE::ARCFIRE) :
		FireInArc(entity->GetPosition(), entity->GetAimDir(), arc, nrOfBullets, entity->GetDamage());
		break;
	}
}
void ProjectileHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay)
{
	trigger_event trigger;
	switch (evnt)
	{
	case(Events::UNIQUE_FIRE::REVERSERBULLETS) :

		trigger.timer = triggerDelay;
		trigger.start = this->projectiles.size();
		trigger.type = evnt;
		trigger.arc = 0;
		trigger.nrOfProjectiles = 0;

		FireInArc(entity->GetPosition(), entity->GetAimDir(), arc, nrOfBullets, entity->GetDamage());

		trigger.end = this->projectiles.size();
		this->eventsToTrack.push_back(trigger);

		break;
	}
}
void ProjectileHandler::OnNotify(Entity * entity, Events::UNIQUE_FIRE evnt, float arc, int nrOfBullets, float triggerDelay, float arcOnSplit, int projectilesOnSplit)
{
	trigger_event trigger;
	switch (evnt)
	{
	case(Events::UNIQUE_FIRE::SPLITFIRE) :

		trigger.timer = triggerDelay;
		trigger.start = this->projectiles.size();
		trigger.type = evnt;
		trigger.arc = arcOnSplit;
		trigger.nrOfProjectiles = projectilesOnSplit;

		FireInArc(entity->GetPosition(), entity->GetAimDir(), arc, nrOfBullets, entity->GetDamage());

		trigger.end = this->projectiles.size();
		this->eventsToTrack.push_back(trigger);

		break;
	}
}
void ProjectileHandler::OnNotify(Entity * entity, Events::ABILITY_TRIGGER evnt, float arc, int nrOfBullets)
{
}
void ProjectileHandler::OnNotify(Entity * entity, Events::PICKUP evnt)
{
}
void ProjectileHandler::FireInArc(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 dir, float arc, int nrOfBullets, unsigned int dmg)
{
	float x = dir.x;
	float y = dir.y;
	float z = dir.z;
	float fireArc = arc;

	if (nrOfBullets == 1)
	{
		fireArc = 0;
	}

	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(-fireArc / 2);
	DirectX::XMVECTOR dirr = DirectX::XMVectorSet(dir.x, dir.y, dir.z, 0.0f);

	dirr = DirectX::XMVector3Transform(dirr, rotation);
	dirr = DirectX::XMVector3Normalize(dirr);

	int nrOfProjectiles = nrOfBullets;

	rotation = DirectX::XMMatrixRotationY(fireArc / (nrOfProjectiles - 1));

	for (int i = 0; i < nrOfProjectiles; i++)
	{
		x = DirectX::XMVectorGetX(dirr);
		y = DirectX::XMVectorGetY(dirr);
		z = DirectX::XMVectorGetZ(dirr);

		this->projectiles.push_back(new Projectile(dmg));
		this->projectiles.at(this->projectiles.size() - 1)->Initialize(&this->m_ball, nullptr, pos.x, pos.z, DirectX::XMFLOAT3(x, y, z));

		dirr = DirectX::XMVector3Transform(dirr, rotation);
	}
}
void ProjectileHandler::triggerEvent(trigger_event &evnt, float arc, int nrOfBullets)
{
    int projectilesToErase = 0;
	DirectX::XMFLOAT3 dir;
	DirectX::XMFLOAT3 pos;
	float arcSplit = 0;
	float nrOfProjectiles = 0;

	for (int i = evnt.start; i < evnt.end; i++)
	{
        if (evnt.end < evnt.start)
        {
            break;
        }
        else if(i < this->projectiles.size())
        {
            switch (evnt.type)
            {
			case(Events::UNIQUE_FIRE::SPLITFIRE) :

                dir = this->projectiles.at(i)->getMoveDir();
                pos = this->projectiles.at(i)->getPos();

				arcSplit = evnt.arc;
				nrOfProjectiles = evnt.nrOfProjectiles;

                this->FireInArc(pos, dir, arcSplit, nrOfProjectiles, 25); //constant damage of 25 atm


                break;
			case(Events::UNIQUE_FIRE::REVERSERBULLETS) :

                dir = this->projectiles.at(i)->getMoveDir();

                dir.x *= -1;
                dir.y *= -1;
                dir.z *= -1;

                this->projectiles.at(i)->setMoveDir(dir);

                break;
            }
        }

	}

}

void ProjectileHandler::onProjectileRemoved(int i)
{
    for (int a = 0; a < this->eventsToTrack.size(); a++)
    {
        if (i >= this->eventsToTrack.at(a).start && i < this->eventsToTrack.at(a).end)
        {
            this->eventsToTrack.at(a).end--;
        }
        else if (i <= this->eventsToTrack.at(a).start)
        {
            this->eventsToTrack.at(a).start--;
            this->eventsToTrack.at(a).end--;
        }
    }
}

