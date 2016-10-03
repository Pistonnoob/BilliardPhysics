#include "StageState.h"
#include "GameStateHandler.h"
#include "MyMathLib.h"
#include "Weapon.h"



StageState::StageState()
{
	this->myCamera = CameraHandler();
	this->myParticleHandler = ParticleHandler();

	this->m_car = Model();
	this->m_ball = Model();
	this->m_ground = Model();
	this->m_AI = Ai();

	this->player = Player();
	this->uiHandler = UIHandler();

	this->enemySubject = EntitySubject();
    this->enemyPjHandler = ProjectileHandler();
	this->enemySubject.AddObserver(&this->enemyPjHandler);

	this->playerSubject = EntitySubject();
	this->playerProjectile = ProjectileHandler();

	this->powerUpPointer = nullptr;
	
    this->projectileHandlerSubject = EntitySubject();

	this->exitStage = false;
	this->pauseStage = false;

	this->latestSpawnPoint = 3;

	/*
	setting up the starting points for the powerups
	this will spawn in order 0-4 

	*/

	this->spawnPos.push_back(DirectX::XMFLOAT2 (-35.0f, 35.0f));
	this->spawnPos.push_back(DirectX::XMFLOAT2(35.0f, 35.0f));
	this->spawnPos.push_back(DirectX::XMFLOAT2(-35.0f, -35.0f));
	this->spawnPos.push_back(DirectX::XMFLOAT2(35.0f, -35.0f));

	this->isCompleted = false;
	this->renderUI = false;
}


StageState::~StageState()
{
}

void StageState::Shutdown()
{
	//Release the models
	this->m_car.Shutdown();
	this->m_ball.Shutdown();
	this->m_ground.Shutdown();
    this->portal.Shutdown();
    this->MeleeModel.Shutdown();
    this->RangedModel.Shutdown();
    this->BomberModel.Shutdown();
    this->BossModel.Shutdown();

    this->enemyPjHandler.ShutDown();
    this->enemySubject.ShutDown();

    this->projectileHandlerSubject.ShutDown();

	this->playerSubject.ShutDown();
	this->playerProjectile.ShutDown();
	
	/*delete this->playerSubject;
	delete this->playerProjectile;

	this->playerSubject = nullptr;
	this->playerProjectile = nullptr;*/
	
	this->player.Shutdown();
	this->uiHandler.Shutdown();

	//Release the enemies
	for (int i = 0; i < this->enemies.size(); i++)
	{
		Enemy* enemyTemp = this->enemies.at(i);
		enemyTemp->Shutdown();
		delete enemyTemp;
	}
	this->enemies.clear();

	for (int i = 0; i < this->levels.size(); i++)
	{
		for (int a = 0; a < this->levels.at(i).wave.size(); a++)
		{
			this->levels.at(i).wave.at(a).toSpawn.clear();
		}
		this->levels.at(i).wave.clear();
	}
	this->levels.clear();

	//Releas eyour m_AI
	this->myParticleHandler.Shutdown();
	this->powerUpSubject.ShutDown();

	this->audioH.ShutDown();

	GameState::Shutdown();
}


int StageState::Initialize(GraphicHandler* gHandler, GameStateHandler * GSH)
{
	int result = 0;
	this->timeElapsed = 0.0f;
	this->exitStage = false;

	ID3D11Device* device = gHandler->GetDevice();
	ID3D11DeviceContext* deviceContext = gHandler->GetDeviceContext();

	//Arm thy father
	result = this->InitializeBase(GSH, device, deviceContext);
	if (result)
	{
		//Pull down the visor of epic particle effects
		//A visor is the moving part of a helmet, namely the part that protects the eyes
		this->myParticleHandler.Initialize(device, deviceContext);

		this->enemySubject.AddObserver(&this->myParticleHandler);
		this->playerSubject.AddObserver(&this->myParticleHandler);

		//Initialize Audio
		this->audioH.Initialize(*gHandler->GetActiveWindow());
		this->playerSubject.AddObserver(&this->audioH);
		this->enemySubject.AddObserver(&this->audioH);

		//Open thy eyes!
		bool cameraResult = this->myCamera.Initialize();
		float zoomIn = 1.0f / 6.0f;

		/*this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 10.0f / zoomIn, -7.0f / zoomIn));
		this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));*/

		this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 20.0f / zoomIn, -7.0f / zoomIn));
		//this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 6.0f, -50.0f));

		this->myCamera.SetLookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

		this->myCamera.UpdateCamera();
		if (cameraResult)
			result = 1;

		//Arm thy mind with the knowledge that will lead thy armies to battle!
		this->m_AI = Ai();




		//Add GameData oberver to enemiesSubject
		this->enemySubject.AddObserver(GameData::GetInstance());

		//powerupts and projectile handlers
		this->playerProjectile.Initialize(device, this->m_deviceContext, &this->projectileHandlerSubject);

		this->playerSubject.AddObserver(&this->playerProjectile);
		this->playerSubject.AddObserver(GameData::GetInstance());

		//the player will rise
		//this->player.Initialize(device, deviceContext, "sphere1", "projectile", true, this->playerSubject);
		this->player.Initialize(gHandler, "sphere1", "projectile", true, &this->playerSubject);
		this->player.SetLevel(1);
		this->player.SetWave(1);

		this->enemySubject.AddObserver(GameData::GetInstance());

        this->enemyPjHandler.Initialize(device, this->m_deviceContext, &this->projectileHandlerSubject);
		
		//Form thy armies from the clay!
		this->m_car = Model();
		this->m_ball = Model();

		bool modelResult = this->m_ball.Initialize(device, this->m_deviceContext, "projectile");
		if (!modelResult) 
		{
			return false;
		}

        modelResult = this->MeleeModel.Initialize(device, this->m_deviceContext, "MeleeRobot");
		if (!modelResult) {
			return false;
		}

        modelResult = this->BomberModel.Initialize(device, this->m_deviceContext, "Grenade");
        if (!modelResult) {
            return false;
        }

        modelResult = this->RangedModel.Initialize(device, this->m_deviceContext, "Shuttle");
        if (!modelResult) {
            return false;
        }

        modelResult = this->BossModel.Initialize(device, this->m_deviceContext, "sphere2");
        if (!modelResult)
        {
            return false;
        }

		modelResult = this->m_car.Initialize(device, this->m_deviceContext, "sphere2");
		if (!modelResult)
		{
			return false;
		}

        modelResult = this->portal.Initialize(device, this->m_deviceContext, "portal");
		if (!modelResult) {
			return false;
		}

		DirectX::XMMATRIX worldMatrix;
		/*worldMatrix = DirectX::XMMatrixTranslation(0.0f, -5.0f, 0.0f);
		this->m_ground.SetWorldMatrix(worldMatrix);*/
		worldMatrix = DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f);
		worldMatrix *= DirectX::XMMatrixTranslation(0.0f, -3.5f, 2.0f);
		this->m_car.SetWorldMatrix(worldMatrix);

		PointLight light;
		light.Diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(0.0f, 1.0f, -4.0f, 1.0f);
		light.Attenuation = DirectX::XMFLOAT4(50.0f, 1.0f, 0.18f, 0.032f);
		this->pointLights.push_back(light);

        //point lights for the portals 
		light.Diffuse = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(-5.0f, 1.0f, 2.0f, 1.0f);
        light.Attenuation = DirectX::XMFLOAT4(2.0f, 0.8f, 0.01f, 0.032f);
		this->pointLights.push_back(light);

        light.Diffuse = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Ambient = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Specular = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Position = DirectX::XMFLOAT4(-5.0f, 1.0f, 2.0f, 1.0f);
        light.Attenuation = DirectX::XMFLOAT4(2.0f, 0.8f, 0.01f, 0.032f);
        this->pointLights.push_back(light);

        light.Diffuse = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Ambient = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Specular = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Position = DirectX::XMFLOAT4(-5.0f, 1.0f, 2.0f, 1.0f);
        light.Attenuation = DirectX::XMFLOAT4(2.0f, 0.8f, 0.01f, 0.032f);
        this->pointLights.push_back(light);

        light.Diffuse = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Ambient = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Specular = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
        light.Position = DirectX::XMFLOAT4(-5.0f, 1.0f, 2.0f, 1.0f);
        light.Attenuation = DirectX::XMFLOAT4(2.0f, 0.8f, 0.01f, 0.032f);
        this->pointLights.push_back(light);
        //-------

		light.Diffuse = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(15.0f, 1.0f, 15.0f, 1.0f);
        light.Attenuation = DirectX::XMFLOAT4(50.0f, 1.0f, 1.5f, 0.0f);
		this->pointLights.push_back(light);

		this->uiHandler.Initialize(gHandler);

		this->uiHandler.AddElement(400, 500, 500, 300, "testUI.mtl", 1, false);
		this->uiHandler.AddElement(120, 40, 600, 500, "Continue.mtl", 1, true);
		this->uiHandler.AddElement(20, 20, 600, 385, "Button.mtl", 1, true);
		this->uiHandler.AddElement(20, 20, 600, 410, "Button.mtl", 1, true);
		this->uiHandler.AddElement(20, 20, 600, 435, "Button.mtl", 1, true);

		this->uiHandler.CreateTextHolder(32); //title
		this->uiHandler.CreateTextHolder(32); //killed
		this->uiHandler.CreateTextHolder(32); //time
		this->uiHandler.CreateTextHolder(32); //score
		this->uiHandler.CreateTextHolder(32); //highscore

		this->uiHandler.CreateTextHolder(32); //points
		this->uiHandler.CreateTextHolder(32); //dmg
		this->uiHandler.CreateTextHolder(32); //hp
		this->uiHandler.CreateTextHolder(32); //movespeed

		GameData::InitializeStageStateGD(device, deviceContext, &this->playerSubject);

		this->timeInStage = std::chrono::system_clock::now();
		GameData::GetInstance()->NewStage();

		if (!result)
		{
			return false;
		}
	}

	return result;
}

int StageState::LoadMap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int stageNr)
{
	bool result = true;
	//Place the ground beneeth your feet and thank the gods for their
	//sanctuary from the oblivion below!

	if (stageNr == 1) {
		result = this->m_ground.Initialize(device, deviceContext, "Stage1NoPlanet");
		if (!result) {
			return false;
		}

		ReadFile("Stage1.txt");

		this->currentLevel = 0;
		this->currentWave = 0;
		this->timeToNextWave = this->levels.at(currentLevel).wave.at(currentWave).time;

		//Arm thy armies!
        SpawnWave(this->currentLevel, this->currentWave);
	}
	else if(stageNr == 2){
		result = this->m_ground.Initialize(device, deviceContext, "Stage1NoPlanet");
		if (!result) {
			return false;
		}

		ReadFile("Stage2.txt");

		this->currentLevel = 0;
		this->currentWave = 0;
		this->timeToNextWave = this->levels.at(currentLevel).wave.at(currentWave).time;

		//Arm thy armies!
		SpawnWave(this->currentLevel, this->currentWave);

	}

	//place lighs on portals
	int i = 1;
	for (auto point : this->spawnPoints)
	{
		this->pointLights.at(i).Position = DirectX::XMFLOAT4(point.x, 1.0f, point.z, 1.0f);
		i++;
	}
	
	return 0;
}

int StageState::HandleInput(InputHandler * input)
{
	int result = 1;

	if (input->isKeyPressed(DIK_ESCAPE))
		this->exitStage = true;

	if (input->isKeyPressed(DIK_U)) {
		if (this->renderUI)
			this->renderUI = false;
		else
			this->renderUI = true;
	}

	if (this->renderUI && input->isMouseKeyPressed(0)) {
		this->uiHandler.HandleInput(input);
	}

	return result;
}

int StageState::Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler)
{
	DirectX::XMFLOAT2 pos;
	DirectX::XMMATRIX worldMatrix;

	int result = 1;

	if (!this->pauseStage) {

		if (this->exitStage)
		{
			this->exitStage = false;
			//Pop ourself
			GameState* state = this->m_GSH->PopState();
			//We do not care about the returned state but check if return
			if (state)
			{
				//The state wasn't a nullptr, meaning it didn't have automatic clearing on
				//Something went somewhat wrong here eh?
			}
		}

		float newDT = deltaTime / 1000000;

		HandleWaveSpawning(newDT, this->isCompleted);

		RemoveDeadEnemies();

		this->m_AI.updateActors(this->enemies, this->player.GetPosition(), newDT);


		this->playerProjectile.Update(newDT);
		this->player.Update(input, gHandler, &this->myCamera, deltaTime);

		this->enemyPjHandler.Update(newDT);

		this->myParticleHandler.Update(deltaTime, this->m_deviceContext);

		//update the timers in the powerup objects, which is held int the GameData singleton
		float timeInSecounds = deltaTime * 0.000001;
		this->timeElapsed += timeInSecounds;
		GameData::Update(timeInSecounds);

		if (this->timeElapsed > 15.0f)
		{
			this->powerUpPointer = GameData::GetRandomPowerup();
			pos = this->spawnPos.at(this->latestSpawnPoint);
			this->latestSpawnPoint++;
			this->latestSpawnPoint %= this->spawnPoints.size();
			this->powerUpPointer->SetPosition(pos.x,pos.y);
			this->timeElapsed = 0;
		}

		

		//Player - projectile intersection
		if (this->enemyPjHandler.IntersectionTest(&this->player)) {

			if (!this->player.IsAlive()) {
				this->exitStage = true;
			}
		}

		//Enemy - projectile intersection
		int i = -1;
		for (auto enemy : this->enemies) {
			i++;

			if (this->playerProjectile.IntersectionTest(enemy)) {

			}
		}
	}
		//Enemy - Player intersection
		for (auto enemy : this->enemies) {

			if (this->player.GetBV()->Intersect(enemy->GetBV())) {

			}
		}

		DirectX::XMFLOAT3 playerPos = this->player.GetPosition();
		this->pointLights.at(0).Position = DirectX::XMFLOAT4(playerPos.x, 1.0f, playerPos.z, 1.0f);

		//if the powerup vector has values, check if intersection has occured
		if (this->powerUpPointer != nullptr)
		{
			this->powerUpPointer->Update(deltaTime);
			if (this->powerUpPointer->GetBV()->Intersect(this->player.GetBV()) == true)
			{
				//nu vill jag säga till gameData att  spelaren har plockat upp denna powerup
				Events::UNIQUE_FIRE powerUp;
				powerUp = this->powerUpPointer->GetType();

				//not very happy with this solution
				switch (powerUp)
				{
				case Events::ARCFIRE:
					this->playerSubject.Notify(&this->player, Events::PICKUP::PICKUP_SPREAD);
					break;
				case Events::SPLITFIRE:
					this->playerSubject.Notify(&this->player, Events::PICKUP::PICKUP_SPITFIRE);
					break;
				case Events::REVERSERBULLETS:
					this->playerSubject.Notify(&this->player, Events::PICKUP::PICKUP_REVERSERBULLETS);
					break;
				default:
					break;
				}

				this->powerUpPointer = nullptr;
			}
		}
	
	//Check if level is completed
	if (this->isCompleted && this->enemies.size() == 0) {
		DirectX::XMFLOAT4 camPos = this->myCamera.GetCameraPos();
		DirectX::XMMATRIX playerDisplay = DirectX::XMMatrixRotationY(-0.5f);;
		playerDisplay *= DirectX::XMMatrixRotationX(1.1f);
		//DirectX::XMMATRIX playerDisplay = DirectX::XMMatrixTranslation(camPos.x - 2.5f, camPos.y - 8.0f, camPos.z + 3.0f);
		//playerDisplay *= DirectX::XMMatrixRotationX(3.0f);
		playerDisplay *= DirectX::XMMatrixTranslation(camPos.x - 2.5f, camPos.y - 8.0f, camPos.z + 3.0f);
		this->player.GetModel()->SetWorldMatrix(playerDisplay);

		//DirectX::XMMATRIX weaponWorldMatrix = playerDisplay;
		////weaponWorldMatrix *= DirectX::XMMatrixRotationY(DirectX::XM_PI / 2);
		//weaponWorldMatrix *= DirectX::XMMatrixTranslation(1.0f, 1.0f, 0.0f);
		//this->player.GetWeapon()->GetModel()->SetWorldMatrix(weaponWorldMatrix);

		std::string text = "";
		if (!this->renderUI) {
			this->renderUI = true;
			this->pauseStage = true;
			GameData::GetInstance()->EndStage(true);

			text = "Level " + std::to_string(this->currentLevel) + " completed!";
			this->uiHandler.UpdateTextHolder(0, text, 320, 90, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 4.0f);

			text = "Mobs killed: " + std::to_string(GameData::GetInstance()->GetEnemiesKilledInStage());
			this->uiHandler.UpdateTextHolder(1, text, 375, 225, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);

			std::chrono::duration<double> elaspedTime = std::chrono::system_clock::now() - this->timeInStage;
			text = "Time: " + std::to_string((int)elaspedTime.count());
			this->uiHandler.UpdateTextHolder(2, text, 375, 250, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);

			text = "Score: " + std::to_string(GameData::GetInstance()->GetScoreInStage());
			this->uiHandler.UpdateTextHolder(3, text, 375, 275, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);

			text = "Highscore: " + std::to_string(GameData::GetInstance()->GetHighScore());
			this->uiHandler.UpdateTextHolder(4, text, 375, 300, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);
		}
		text = "Points to spend: " + std::to_string(GameData::GetInstance()->GetPoints());
		this->uiHandler.UpdateTextHolder(5, text, 375, 350, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);
		text = "Damage: " + std::to_string(GameData::GetInstance()->GetPlayerDamage());
		this->uiHandler.UpdateTextHolder(6, text, 375, 375, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);
		text = "Health: " + std::to_string(GameData::GetInstance()->GetPlayerHealth());
		this->uiHandler.UpdateTextHolder(7, text, 375, 400, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);
		text = "Speed: " + std::to_string(GameData::GetInstance()->GetPlayerMoveSpeed());
		this->uiHandler.UpdateTextHolder(8, text, 375, 425, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 1.5f);

		if (this->uiHandler.WasButtonPressed(1)) {
			this->pauseStage = false;
			this->exitStage = true;	//end stage for testing
		}
		if (this->uiHandler.WasButtonPressed(2)) {
			GameData::GetInstance()->SpendPointOn(0);
		}
		if (this->uiHandler.WasButtonPressed(3)) {
			GameData::GetInstance()->SpendPointOn(1);
		}
		if (this->uiHandler.WasButtonPressed(4)) {
			GameData::GetInstance()->SpendPointOn(2);
		}
	}

	return result;
}

int StageState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	int result = 0;
	DirectX::XMFLOAT3 pos;
    DirectX::XMMATRIX worldMatrix;

	//Render models

	//Player and weapon render
	gHandler->DeferredRender(this->player.GetModel(), &this->myCamera);
	gHandler->DeferredRender(this->player.GetWeapon()->GetModel(), &this->myCamera);

    for (auto point : this->spawnPoints)
    {
        worldMatrix = DirectX::XMMatrixTranslation(point.x, point.y, point.z);
        this->portal.SetWorldMatrix(worldMatrix);
        gHandler->DeferredRender(&this->portal, &this->myCamera);
    }


	//render powerup if any on screen
	if (this->powerUpPointer != false)
	{
		gHandler->DeferredRender(powerUpPointer->GetModel(), &this->myCamera);
	}

	//renders all the actors in the enemies vector
    Entity* temp;
	for (int i = 0; i < this->enemies.size(); i++)
	{
        temp = this->enemies.at(i);
		pos = temp->GetPosition();

        DirectX::XMFLOAT3 dirVec = temp->GetAimDir();
		float angle = atan2(dirVec.z, dirVec.x);

        if (temp->GetType() == Type::MELEEE)
        {
            angle += 3.14f / 2;
        }

        DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationY(-angle);

		worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		//this->m_car.SetWorldMatrix(rotMatrix * worldMatrix);
        temp->GetModel()->SetWorldMatrix(rotMatrix * worldMatrix);

		gHandler->DeferredRender(temp->GetModel(), &this->myCamera);

	}

	gHandler->DeferredRender(&this->m_ground, &this->myCamera);
	
	//render shots
    this->enemyPjHandler.Render(gHandler, &this->myCamera);
	this->playerProjectile.Render(gHandler, &this->myCamera);
	
	//render that shiet
	//gHandler->DeferredRender(this->spreadPower.GetModel(), &this->myCamera);

	for (int i = 0; i < this->enemies.size(); i++)
	{

		worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		this->m_car.SetWorldMatrix(worldMatrix);

		gHandler->ShadowRender(&this->m_car, &this->myCamera);
	}


	gHandler->ShadowRender(this->player.GetModel(), &this->myCamera);
	gHandler->ShadowRender(this->player.GetWeapon()->GetModel(), &this->myCamera);
	//gHandler->ShadowRender(&this->m_ground, &this->myCamera);

	gHandler->LightRender(this->myCamera.GetCameraPos(), this->pointLights);

	this->myParticleHandler.Render(gHandler, &this->myCamera);

	gHandler->UIRender(this->player.GetUIHandler());
	if (this->renderUI)
		gHandler->UIRender(&this->uiHandler);

	return result;
}


void StageState::ReadFile(std::string fileName)
{
	std::string line;
	std::ifstream myFile("..\\SmallGameProject\\Resources\\Data\\" + fileName);

    if (myFile.is_open())
    {
		int nrOfSpawnPoints = 0;
		std::getline(myFile, line);

		std::string xStr;
		std::string zStr;
		while(line.at(0) == 'p')
		{
			std::stringstream ss;
			DirectX::XMFLOAT3 point;
			point.y = 0;
			size_t start = 1;
			size_t end = line.find(",");
			xStr = line.substr(start, end - 1);
			ss << xStr;
			ss >> point.x;
			ss.clear();

			start = end + 1;
			end = line.find("}");
			zStr = line.substr(start, end - 1);
			zStr.pop_back();
			ss << zStr;
			ss >> point.z;
			ss.clear();

			nrOfSpawnPoints++;
			this->spawnPoints.push_back(point);
			getline(myFile, line);
		}
		while (line == "new Level")
		{
			getline(myFile, line);
			Level temp;
			this->levels.push_back(temp);
			while (line != "new Level" && line != "end")
			{
				Wave waveTemp;
				ToSpawn spawnTemp;
				std::string waveLenght = "";
				std::string enemyType = "";
				std::string nrOfEnemies = "";

				//get wave lenght
				size_t start = 1;
				size_t end = line.find("}");
				waveLenght = line.substr(start, end - 1);

				std::stringstream ss(waveLenght);
				ss >> waveTemp.time;
				ss.clear();

				getline(myFile, line);
				for (int i = 0; i < nrOfSpawnPoints; i++)
				{
                    spawnTemp.spawnIndex = 0;
					spawnTemp.type = Type::NONE;
					if (line.at(1) == '}')
					{
					}
					else if (line.at(0) == '{')
					{
                        for (int a = 1; a < line.size(); a++)
                        {
                            if (line.at(a) != ' ' && line.at(a) != '}')
                            {
								std::string bossCheck = line.substr(a, a + 3);
                                if (bossCheck.at(bossCheck.size() - 1) == '}')
                                {
                                    bossCheck.pop_back();
                                }
                                if (bossCheck == "BOSS")
                                {
                                    spawnTemp.spawnIndex = i;
                                    spawnTemp.type = ConvertToEnemyType(bossCheck);
                                    waveTemp.toSpawn.push_back(spawnTemp);
                                    a += 3;
                                }
                                else
                                {
                                    spawnTemp.spawnIndex = i;
                                    enemyType = line.at(a);
                                    spawnTemp.type = ConvertToEnemyType(enemyType);
                                    waveTemp.toSpawn.push_back(spawnTemp);
                                }

                            }
                        }
					}


					getline(myFile, line);
				}
				this->levels.at(this->levels.size() - 1).wave.push_back(waveTemp);
			}

		}


    }
}

void StageState::HandleWaveSpawning(float deltaTime, bool& isCompleted)
{
    this->timeToNextWave -= deltaTime;
    if (this->enemies.size() == 0)
    {
        this->timeToNextWave = 0;
    }
    if (this->timeToNextWave <= 0)
    {
        if (this->currentLevel < this->levels.size())
        {
            this->currentWave++;
			this->player.SetWave(this->currentWave + 1);
            if (this->currentWave >= this->levels.at(this->currentLevel).wave.size())
            {
                this->currentWave = 0;
                this->currentLevel++;
				this->player.SetWave(this->currentWave + 1);
				this->player.SetLevel(this->currentLevel + 1);
            }
            if (this->currentLevel < this->levels.size())
            {
                this->timeToNextWave = this->levels.at(this->currentLevel).wave.at(this->currentWave).time;
                SpawnWave(this->currentLevel, this->currentWave);
            }
			else {
				//If the Level has spawned all waves
				isCompleted = true;
			}
        }
    }
}


void StageState::SpawnWave(int levelIndex, int waveIndex)
{
	//level
	// |->wave
	//      |->time
	//      |->toSpawn
	//           |->type
	//			 |->amnount

    int size = this->levels.at(levelIndex).wave.at(waveIndex).toSpawn.size();
    for (int i = 0; i < size; i++)
    {
        Type type = this->levels.at(levelIndex).wave.at(waveIndex).toSpawn.at(i).type;
        int point = this->levels.at(levelIndex).wave.at(waveIndex).toSpawn.at(i).spawnIndex;
        SpawnEnemy(type, point);
    }
}
void StageState::SpawnEnemy(Type type, int pointIndex)
{
	int x = this->spawnPoints.at(pointIndex).x;
	int z = this->spawnPoints.at(pointIndex).z;
	switch (type)
	{
	case(Type::BOMBER) :
		this->enemies.push_back(new BomberEnemy(x, z));
		this->enemies.at(this->enemies.size() - 1)->Initialize(&this->BomberModel, &this->enemySubject, true);
		break;
	case(Type::RANGED) :
		this->enemies.push_back(new RangedEnemy(x, z));
		this->enemies.at(this->enemies.size() - 1)->Initialize(&this->RangedModel, &this->enemySubject, true);
		break;
	case(Type::MELEEE) :
		this->enemies.push_back(new MeleeEnemy(x, z));
		this->enemies.at(this->enemies.size() - 1)->Initialize(&this->MeleeModel, &this->enemySubject, true);
		break;
	case(Type::BOSS) :
		Boss* boss = new Boss(x, z);
		boss->Initialize(&this->BossModel, &this->enemySubject, true, 1);
		this->enemies.push_back(boss);
		break;
	}
}
void StageState::RemoveDeadEnemies()
{
    for (int i = 0; i < this->enemies.size(); i++)
    {
        if (!(this->enemies.at(i)->IsAlive()))
        {
            this->enemies.at(i)->Shutdown();
            delete this->enemies.at(i);
            this->enemies.erase(this->enemies.begin() + i);
            i--;

        }
    }
}
Type StageState::ConvertToEnemyType(std::string type)
{
	if (type == "B")
	{
		return Type::BOMBER;
	}
	else if (type == "R")
	{
		return Type::RANGED;
	}
	else if (type == "M")
	{
		return Type::MELEEE;
	}
	else if(type == "BOSS")
	{
		return Type::BOSS;
	}
	else
	{
		return Type::NONE;
	}
}
