#include "BilliardState.h"
#include "GameStateHandler.h"



BilliardState::BilliardState()
{
	this->myCamera = CameraHandler();

	this->m_cueBall = Model();
	this->m_8Ball = Model();
	this->m_cueStick = Model();
	this->m_table = Model();

	this->simulationCompleted = true;
	this->exitStage = false;
	this->manualClearing = false;
	

}


BilliardState::~BilliardState()
{
}

void BilliardState::Shutdown()
{
	this->m_cueBall.Shutdown();
	this->m_8Ball.Shutdown();
	this->m_cueStick.Shutdown();
	this->m_table.Shutdown();

	GameState::Shutdown();
}

int BilliardState::Initialize(GraphicHandler * gHandler, GameStateHandler * GSH)
{
	int result = 0;

	this->exitStage = false;
	this->simulationCompleted = true;
	this->manualClearing = false;

	this->billiardCatchers[0].radius = 6.5f * SCALING;
	this->billiardCatchers[1].radius = 6.5f * SCALING;
	this->billiardCatchers[2].radius = 10.0f * SCALING;
	this->billiardCatchers[3].radius = 10.0f * SCALING;
	this->billiardCatchers[4].radius = 10.0f * SCALING;
	this->billiardCatchers[5].radius = 10.0f * SCALING;

	this->billiardCatchers[0].pos.x = -72.300f * SCALING;
	this->billiardCatchers[1].pos.x = 72.300f * SCALING;
	this->billiardCatchers[2].pos.x = -70.0f * SCALING;
	this->billiardCatchers[3].pos.x = 70.0f * SCALING;
	this->billiardCatchers[4].pos.x = -70.0f * SCALING;
	this->billiardCatchers[5].pos.x = 70.0f * SCALING;
	this->billiardCatchers[0].pos.z = 0.0f * SCALING;
	this->billiardCatchers[1].pos.z = 0.0f * SCALING;
	this->billiardCatchers[2].pos.z = -133.0f * SCALING;
	this->billiardCatchers[3].pos.z = -133.0f * SCALING;
	this->billiardCatchers[4].pos.z = 133.0f * SCALING;
	this->billiardCatchers[5].pos.z = 133.0f * SCALING;

	this->activeBall.pos = DirectX::XMFLOAT3(0.0f * SCALING, 1.0f * SCALING, 0.0f * SCALING);
	this->activeBall.direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->activeBall.radius = 0.0476f;
	this->activeBall.mass = 0.17f;
	this->activeBall.density = 1700;

	for (int i = 0; i < OTHER_BALL_COUNT; i++)
	{
		this->otherBalls[i].pos = DirectX::XMFLOAT3(0.0f * SCALING, 1.0f * SCALING, 1.0f * SCALING);
		this->otherBalls[i].direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->otherBalls[i].radius = 0.0508f;
		this->otherBalls[i].mass = 0.16f;
		this->otherBalls[i].density = 1700;
	}

	ID3D11Device* device = gHandler->GetDevice();
	ID3D11DeviceContext* deviceContext = gHandler->GetDeviceContext();

	//Initialize the base class GameState
	result = this->InitializeBase(GSH, device, deviceContext);
	if (result)
	{
		//Proceed to initialize thyself
		//Firstly thy must create thy mighty sword of obj!
		bool modelResult = this->m_cueBall.Initialize(device, this->m_deviceContext, "CueBall");
		if (!modelResult)
		{
			return -1;
		}

		modelResult = this->m_8Ball.Initialize(device, this->m_deviceContext, "8Ball");
		if (!modelResult) {
			return -1;
		}

		modelResult = this->m_cueStick.Initialize(device, this->m_deviceContext, "CueStick");
		if (!modelResult) {
			return -1;
		}

		modelResult = this->m_table.Initialize(device, this->m_deviceContext, "BilliardBoard");
		if (!modelResult) {
			return -1;
		}
		//Secondly thy must lift why mighty sword of obj! (move, scale and rotate the objects)
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixScaling(SCALING, SCALING, SCALING);
		this->m_cueBall.SetWorldMatrix(worldMatrix);
		this->m_8Ball.SetWorldMatrix(worldMatrix);
		this->m_cueStick.SetWorldMatrix(worldMatrix);
		this->m_table.SetWorldMatrix(worldMatrix);

		//Open thy eyes!
		bool cameraResult = this->myCamera.Initialize();
		float zoomIn = (1.0f / 10.0f) / SCALING;

		/*this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 10.0f / zoomIn, -7.0f / zoomIn));
		this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));*/

		//this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 20.0f / zoomIn, -7.0f / zoomIn));
		this->myCamera.SetCameraPos(DirectX::XMFLOAT3(-12.0f / zoomIn, 26.0f / zoomIn, 0.0f));
		//this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 6.0f, -50.0f));

		this->myCamera.SetLookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

		this->myCamera.UpdateCamera();
		if (cameraResult)
			result = 1;

		//And the developer said "Let there be light!", and so light came to be
		PointLight light;
		light.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(0.0f, 20.0f, -4.0f, 1.0f);
		light.Attenuation = DirectX::XMFLOAT4(50.0f, 1.0f, 0.09f, 0.032f);
		this->pointLights.push_back(light);

	}

	return result;
}

int BilliardState::HandleInput(InputHandler * input)
{
	int result = 1;

	if (input->isKeyPressed(DIK_ESCAPE))
		this->exitStage = true;
	
	return result;
}

int BilliardState::Update(float deltaTime, InputHandler * input, GraphicHandler * gHandler)
{
	int result = 0;
	result = 1;

	if (this->exitStage)
	{
		this->exitStage = false;
		GameState* state = this->m_GSH->PopState();
		//We do not care about the returned state but check if return
		if (state)
		{
			//The state wasn't a nullptr, meaning it didn't have automatic clearing on
			//Something went somewhat wrong here eh?
		}
	}
	else
	{
		//If the simulation of movement has yet to complete
		//If the simulation is complete, all balls will be still
		if (!this->simulationCompleted)
		{
			//Update the balls

			//Move the balls
			this->activeBall.pos = DirectX::XMFLOAT3(this->activeBall.pos.x + this->activeBall.direction.x * this->activeBall.power * deltaTime,
				this->activeBall.pos.y + this->activeBall.direction.y * this->activeBall.power * this->activeBall.power * deltaTime,
				this->activeBall.pos.z + this->activeBall.direction.z * this->activeBall.power * deltaTime);
			for (int i = 0; i < OTHER_BALL_COUNT; i++)
			{
				this->otherBalls[i].pos = DirectX::XMFLOAT3(this->otherBalls[i].pos.x + this->otherBalls[i].direction.x * this->activeBall.power * deltaTime,
					this->otherBalls[i].pos.y + this->otherBalls[i].direction.y * this->activeBall.power * deltaTime,
					this->otherBalls[i].pos.z + this->otherBalls[i].direction.z * this->activeBall.power * deltaTime);
			}
			//Check collisions

				//Correct collisions
					//transfer velocities

			//Check if no balls are moving.
			bool ballMoving = false;
			if (this->activeBall.power > 0.000001f)
			{
				ballMoving = true;
			}
			for (int i = 0; i < OTHER_BALL_COUNT && !ballMoving; i++)
			{
				ballMoving = this->otherBalls[i].power > 0.000001f;
			}
			this->simulationCompleted = !ballMoving;
			//Set the positions of the ball models to be accurate
			DirectX::XMMATRIX posOffset;
			posOffset = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&this->activeBall.pos));
			
			this->m_cueBall.SetWorldMatrix(posOffset);
			posOffset = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&this->otherBalls[0].pos));
			//posOffset = DirectX::XMMatrixMultiply(posOffset, DirectX::XMMatrixScaling(SCALING, SCALING, SCALING));
			this->m_8Ball.SetWorldMatrix(posOffset);
		}
		else
		{
			//If the balls are all stationary, prepare for another shot from the active ball
			//Check the mouse position compared to the ball center
			//Use that data to calculate a direction matrix to use for the ball
			//Set the power of the ball
			//Start simulating ball movement again
			this->simulationCompleted = false;
		}
	}

	return result;
}

int BilliardState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	int result = 0;
	result = 1;

	gHandler->DeferredRender(&this->m_table, &this->myCamera);
	gHandler->DeferredRender(&this->m_cueStick, &this->myCamera);
	gHandler->DeferredRender(&this->m_cueBall, &this->myCamera);
	gHandler->DeferredRender(&this->m_8Ball, &this->myCamera);

	gHandler->LightRender(this->myCamera.GetCameraPos(), this->pointLights);


	return result;
}
