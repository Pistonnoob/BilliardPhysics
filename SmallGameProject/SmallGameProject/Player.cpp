#include "Player.h"

Player::Player() : Actor()
{
	this->posX = 0.f;
	this->posZ = 0.f;
	this->playerMovmentSpeed = 100;
	this->health = 100;	
	this->damage = 100;

	//Initiliaze the forward vecktor as 0,0,1
	this->forwardDir = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 1));
	this->playerWeapon = nullptr;

	this->uiHandler = UIHandler();
}

Player::~Player()
{

}

bool Player::Initialize(GraphicHandler* graphicsH, std::string playerModelFilename,
	std::string weaponModelFile, bool isSphere, EntitySubject* entitySub)
{
	ID3D11Device* device = graphicsH->GetDevice();
	ID3D11DeviceContext* deviceContext = graphicsH->GetDeviceContext();

	if (!Entity::Initialize(device, deviceContext, playerModelFilename, isSphere, entitySub)) {
		return false;
	}
	this->entitySubject->Notify(this, Events::ENTITY::PLAYER_CREATED);
	this->playerWeapon = new Weapon();
	if (!this->playerWeapon->Initialize(device, deviceContext, weaponModelFile)) {
		return false;
	}

	//Rotation matrix
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(0);

	//Set the weapon pos to the Player
	//Set the world matrix to a defualt state
	DirectX::XMMATRIX playerWorldMatrix;
	this->entityModel->GetWorldMatrix(playerWorldMatrix);
	
		
	//Initial offset
	DirectX::XMMATRIX offset = DirectX::XMMatrixTranslation(this->posX, 0, this->posZ);
	//create the new world matrix
	playerWorldMatrix = playerWorldMatrix * offset;

	//Set the model matrix
	this->entityModel->SetWorldMatrix(playerWorldMatrix);

	//weapon matrix
	DirectX::XMMATRIX weaponWorldMatrix = playerWorldMatrix;
	offset = DirectX::XMMatrixTranslation(5, 4, 0);
	weaponWorldMatrix = weaponWorldMatrix * offset;

	//give the player model its new 
	this->playerWeapon->GetModel()->SetWorldMatrix(weaponWorldMatrix);

	this->uiHandler.Initialize(graphicsH);
	this->uiHandler.CreateTextHolder(32);
	this->uiHandler.CreateTextHolder(32);
	this->uiHandler.CreateTextHolder(32);
	this->uiHandler.CreateTextHolder(32);
	this->uiHandler.CreateTextHolder(32);

	return true;
}

void Player::Shutdown()
{
	if (this->playerWeapon) {
		this->playerWeapon->ShutDown();
		delete this->playerWeapon;
		this->playerWeapon = nullptr;
	}

	this->uiHandler.Shutdown();

	Entity::Shutdown(false);
}

Weapon * Player::GetPlayerWeapon()
{
	return this->playerWeapon;
}

void Player::PowerPickup(const int & POWER_ENUM)
{
}

void Player::HandleInput(InputHandler * input, float dTime)
{
	if (input->isKeyPressed(DIK_1))
	{
		this->entitySubject->Notify(this, Events::ENTITY::BOMBER_DEAD);
	}
	if (input->isKeyPressed(DIK_2))
	{
		this->entitySubject->Notify(this, Events::ENTITY::PLAYER_HIT);
	}
	if (input->isKeyPressed(DIK_3))
	{
		this->entitySubject->Notify(this, Events::ENTITY::RANGED_CREATED);
	}
	if (input->isKeyPressed(DIK_4))
	{
		this->entitySubject->Notify(this, Events::ENTITY::RANGED_DEAD);
	}
	if (input->isKeyDown(DIK_W)) {
		this->MoveUp(dTime);
	}
	if (input->isKeyDown(DIK_S)) {
		this->MoveDown(dTime);
	}
	if (input->isKeyDown(DIK_D)) {
		this->MoveRight(dTime);
	}
	if (input->isKeyDown(DIK_A)) {
		this->MoveLeft(dTime);
	}

	if (input->isKeyPressed(DIK_C))
	{
		//how do I update this shiet
		//this->entitySubject->Notify(this, Events::PICKUP::POWERUP_PICKUP);
	}

	if(input->isMouseKeyPressed(0))	//0 = left, 1 = right, 2 = scroll click, 3 = "Down button" on mouse
	{
		this->Fire(0.0);
	}
}

void Player::Update(InputHandler* input, GraphicHandler* gHandler, CameraHandler* cameraH, float deltaTime)
{

	this->HandleInput(input, deltaTime);

	DirectX::XMMATRIX playerWorldMatrix;
	this->entityModel->GetWorldMatrix(playerWorldMatrix);

	//Initial offset
	DirectX::XMMATRIX offset = DirectX::XMMatrixTranslation(this->posX, 0, this->posZ);
	//create the new world matrix
	playerWorldMatrix = offset;

	//Set the model matrix
	this->entityModel->SetWorldMatrix(playerWorldMatrix);

	this->RotatePlayerTowardsMouse(input->getMousePos(), gHandler, cameraH);

	//Update the bounding box pos and rotation
	this->entityModel->GetWorldMatrix(playerWorldMatrix);
	this->entityBV->UpdateBoundingVolume(playerWorldMatrix);

	//weapon matrix
	this->entityModel->GetWorldMatrix(playerWorldMatrix);
	DirectX::XMMATRIX weaponWorldMatrix = playerWorldMatrix;
	offset = DirectX::XMMatrixTranslation(2.5f, 1.0f, 0.0f);
	weaponWorldMatrix = offset * weaponWorldMatrix;

	this->playerWeapon->GetModel()->SetWorldMatrix(weaponWorldMatrix);

	std::string text = "Damage: " + std::to_string(this->damage + GameData::GetInstance()->GetPlayerDamage());
	this->uiHandler.UpdateTextHolder(0, text, 150, 20, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.5f);
	text = "Health: " + std::to_string((this->health + GameData::GetInstance()->GetPlayerHealth()) - this->damageTaken);
	this->uiHandler.UpdateTextHolder(1, text, 270, 20, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.5f);
	text = "Speed: " + std::to_string(this->playerMovmentSpeed + GameData::GetInstance()->GetPlayerMoveSpeed());
	this->uiHandler.UpdateTextHolder(2, text, 390, 20, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.5f);


	GameData::Update(deltaTime);
}

Weapon * Player::GetWeapon()
{
	return this->playerWeapon;
}


void Player::MoveRight(float deltaTime)
{
	if (this->posX < 42.0f) {
		this->posX += (0.0000005f * deltaTime * (this->playerMovmentSpeed + GameData::GetInstance()->GetPlayerMoveSpeed()));
	}
}


void Player::MoveLeft(float deltaTime)
{
	if (this->posX > -42.0f) {
		this->posX -= (0.0000005f * deltaTime * (this->playerMovmentSpeed + GameData::GetInstance()->GetPlayerMoveSpeed()));
	}
}


void Player::MoveUp(float deltaTime)
{
	if (this->posZ < 42.0f) {
		this->posZ += (0.0000005f * deltaTime * (this->playerMovmentSpeed + GameData::GetInstance()->GetPlayerMoveSpeed()));
	}
}


void Player::MoveDown(float deltaTime)
{
	if (this->posZ > -42.0f) {
		this->posZ -= (0.0000005f * deltaTime  * (this->playerMovmentSpeed + GameData::GetInstance()->GetPlayerMoveSpeed()));
	}
}

void Player::Move(DirectX::XMFLOAT3 moveVec)
{
	this->posX += moveVec.x;
	this->posZ += moveVec.z;
}

void Player::Fire(float deltaT)
{
	DirectX::XMStoreFloat3(&this->aimDir, this->forwardDir);

	this->aimDir.x = DirectX::XMVectorGetX(this->forwardDir);
	this->aimDir.z = DirectX::XMVectorGetY(this->forwardDir);
	this->aimDir.y = 0.0f;

	if (GameData::getNrOfActivePowerups() != 0)
	{
		std::list<PowerUp*> activePows = GameData::getPowerup();
		this->playerWeapon->ShootWeapon(this, activePows.front()->GetType());
	}
	
	else
	{
		this->playerWeapon->ShootWeapon(this);
	}
	
}

void Player::RotatePlayerTowardsMouse(DirectX::XMFLOAT2 mousePos, GraphicHandler* gHandler, CameraHandler* cameraH)
{
	// The angle is calculated in Normal Device Space

	DirectX::XMVECTOR playerPos = DirectX::XMVectorSet(this->posX, 0, this->posZ, 1);
	DirectX::XMMATRIX modelWorld;
	DirectX::XMMATRIX cameraView;
	DirectX::XMMATRIX projection;
	int screenWidth = gHandler->GetScreenWidth();
	int screenHeight = gHandler->GetScreenHeight();

	this->entityModel->GetWorldMatrix(modelWorld);
	cameraH->GetViewMatrix(cameraView);
	projection = gHandler->GetPerspectiveMatrix();
	
	//DirectX::XMMATRIX clipMatrix = modelWorld * cameraView * projection;

	//playerPos = DirectX::XMVector3TransformCoord(playerPos, modelWorld);
	playerPos = DirectX::XMVector4Transform(playerPos, cameraView);
	playerPos = DirectX::XMVector4Transform(playerPos, projection);

	//Move player pos to a float4 to be able to devide each value with w
	DirectX::XMFLOAT4 v;
	DirectX::XMStoreFloat4(&v, playerPos);

	v.x = v.x / v.w;
	v.y = v.y / v.w;
	v.z = v.z / v.w;

	// Re-save it
	playerPos = DirectX::XMLoadFloat4(&v);

	DirectX::XMFLOAT4X4 tempProj;
	DirectX::XMStoreFloat4x4(&tempProj, projection);

	//Move the cords to the window
	float mouseX = mousePos.x - (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	float mouseY = mousePos.y - (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	// Get the mouse pos in -1, 1
	mouseX = (((2 * mouseX) / screenWidth) - 1);
	mouseY = (((-2 * mouseY) / screenHeight) + 1);

	//Move the pos to a vector
	DirectX::XMVECTOR mousePosV = DirectX::XMVectorSet( mouseX, mouseY , 1, 1);
	//DirectX::XMVECTOR mousePosV = DirectX::XMVectorSet(mouseX, 0, mouseY, 1);
	
	//Direction vector
	DirectX::XMVECTOR dirVec = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(mousePosV, playerPos));
	this->forwardDir = dirVec;
	float angle = atan2(DirectX::XMVectorGetY(dirVec), DirectX::XMVectorGetX(dirVec));
	
	//Create the rotation matrix
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(-angle);

	//Apply rotation
	DirectX::XMMATRIX weaponModelMatrix;
	DirectX::XMMATRIX playermodelMatrix;
	this->playerWeapon->GetModel()->GetWorldMatrix(weaponModelMatrix);
	this->entityModel->GetWorldMatrix(playermodelMatrix);

	//this->playerWeapon->GetModel()->SetWorldMatrix(rotationMatrix * weaponModelMatrix);
	this->entityModel->SetWorldMatrix(rotationMatrix * playermodelMatrix);

}

unsigned int Player::GetDamage()
{
	return this->damage + GameData::GetInstance()->GetPlayerDamage();
}

bool Player::IsAlive()
{
	if ((this->health + GameData::GetInstance()->GetPlayerHealth()) - this->damageTaken <= 0) {
		return false;
	}

	return true;
}

UIHandler * Player::GetUIHandler()
{
	return &this->uiHandler;
}

void Player::SetLevel(int level)
{
	std::string text = "Level: " + std::to_string(level);
	this->uiHandler.UpdateTextHolder(3, text, 590, 20, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.5f);
}

void Player::SetWave(int wave)
{
	std::string text = "Wave: " + std::to_string(wave);
	this->uiHandler.UpdateTextHolder(4, text, 680, 20, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.5f);
}
