#include "Entity.h"

Entity::Entity() {
	this->entityModel = nullptr;
	this->entityBV = nullptr;

	this->posX = 0.0f;
	this->posZ = 0.0f;

	this->damageTaken = 0;
}

Entity::~Entity()
{

}

bool Entity::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::string objFilename, bool isSphere, EntitySubject* entitySub)
{
	this->entitySubject = entitySub;
	this->entityModel = new Model();
	if (!this->entityModel->Initialize(device, deviceContext, objFilename))
	{
		return false;
	}
	
	//Generate the Bounding volume
	if (isSphere) {
		this->entityBV = new SphereBoundingVolume();
		this->entityBV->GenerateBounds(this->entityModel);
	}
	else {
		this->entityBV = new BoxBoundingVolume();
		this->entityBV->GenerateBounds(this->entityModel);
	}
	DirectX::XMMATRIX modelWorldMatrix;
	this->entityModel->GetWorldMatrix(modelWorldMatrix);
	this->entityBV->UpdateBoundingVolume(modelWorldMatrix);

	return true;
}

bool Entity::Initialize(Model * model, EntitySubject* entitySubject, bool isSphere)
{
    this->entitySubject = entitySubject;

	this->entityModel = model;

	//Generate the Bounding volume
	if (isSphere) {
		this->entityBV = new SphereBoundingVolume();
		this->entityBV->GenerateBounds(this->entityModel);
	}
	else {
		this->entityBV = new BoxBoundingVolume();
		this->entityBV->GenerateBounds(this->entityModel);
	}
	DirectX::XMMATRIX modelWorldMatrix;
	this->entityModel->GetWorldMatrix(modelWorldMatrix);
	this->entityBV->UpdateBoundingVolume(modelWorldMatrix);

	return true;
}

void Entity::Shutdown(bool isEnemy)
 {
	if (this->entityModel && !isEnemy) 
	{
		this->entityModel->Shutdown();
		delete this->entityModel;
		this->entityModel = nullptr;
	}

	if (this->entityBV) 
	{
		delete this->entityBV;
	}
	this->entityBV = nullptr;
}
void Entity::AddObservers(Observer * observer)
{
    this->entitySubject->AddObserver(observer);
}

DirectX::XMFLOAT3 Entity::GetAimDir()
{
	return DirectX::XMFLOAT3(0, 0, 0);
}

EntitySubject * Entity::GetEntitySubject() const
{
	return this->entitySubject;
}

Type Entity::GetType()
{
    return this->myType;
}

void Entity::HandleInput()
{
}

void Entity::Update()
{
}

Model* Entity::GetModel()
{
    return this->entityModel;
}

BoundingVolume* Entity::GetBV()
{
    return this->entityBV;
}

DirectX::XMFLOAT3 Entity::GetPosition()
{
    DirectX::XMFLOAT3 pos;
    pos.x = this->posX;
    pos.y = 0;
    pos.z = this->posZ;
    return pos;
}

void Entity::SetPosition(float newPosX, float newPosZ)
{
	this->posX = newPosX;
	this->posZ = newPosZ;
}

unsigned int Entity::GetDamage()
{
	return this->damage;
}

void Entity::ApplyDamage(int dmg)
{
	this->damageTaken += dmg;
}

bool Entity::IsAlive()
{
	if (this->health - this->damageTaken <= 0) {
		return false;
	}

	return true;
}



