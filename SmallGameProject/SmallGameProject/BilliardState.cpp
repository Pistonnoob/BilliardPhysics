#include "BilliardState.h"
#include "GameStateHandler.h"
#include <math.h>



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
	//Release the models
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
	this->simulationCompleted = false;
	this->manualClearing = false;

	for (int i = 0; i < this->OTHER_BALL_COUNT; i++)
	{
		this->ballState[i] = 1;
	}

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

	this->activeBall.pos = DirectX::XMFLOAT3(CUEBALL_RADIUS + OTHERBALL_RADIUS - 0.01f, 4.0f * SCALING, -0.5f);
	this->activeBall.direction = DirectX::XMFLOAT3(0.0f, 0.0f, 4.00f);
	this->activeBall.radius = 4.0f * SCALING;
	this->activeBall.mass = 0.17f;
	this->activeBall.density = 1700;
	this->activeBall.power = 1.0f;

	for (int i = 0; i < OTHER_BALL_COUNT; i++)
	{
		this->otherBalls[i].pos = DirectX::XMFLOAT3(0.0f * SCALING, 4.2689f * SCALING, 10.0f * SCALING);
		this->otherBalls[i].direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->otherBalls[i].radius = 4.2689f * SCALING;
		this->otherBalls[i].mass = 0.16f;
		this->otherBalls[i].density = 1700;
		this->otherBalls[i].power = 1.0f;
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
		//this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 26.0f / zoomIn, -0.1f / zoomIn));

		this->myCamera.SetLookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

		this->myCamera.UpdateCamera();
		if (cameraResult)
			result = 1;

		//And the developer said "Let there be light!", and so light came to be
		PointLight light;
		light.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(0.0f, 1.0f * SCALING, 0.0f, 1.0f);
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
	deltaTime = deltaTime / 1000000.0f;

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
#pragma region
			//Moving the active ball
			this->activeBall.pos = DirectX::XMFLOAT3(this->activeBall.pos.x + this->activeBall.direction.x * this->activeBall.power * deltaTime,
				this->activeBall.pos.y + this->activeBall.direction.y * this->activeBall.power * this->activeBall.power * deltaTime,
				this->activeBall.pos.z + this->activeBall.direction.z * this->activeBall.power * deltaTime);
			//Moving the other balls
			for (int i = 0; i < OTHER_BALL_COUNT; i++)
			{
				if (this->ballState[i] > 0)
				{
					this->otherBalls[i].pos = DirectX::XMFLOAT3(this->otherBalls[i].pos.x + this->otherBalls[i].direction.x * this->activeBall.power * deltaTime,
						this->otherBalls[i].pos.y + this->otherBalls[i].direction.y * this->activeBall.power * deltaTime,
						this->otherBalls[i].pos.z + this->otherBalls[i].direction.z * this->activeBall.power * deltaTime);
				}
			}
#pragma endregion Moving the balls
			//Check balls within catchers
#pragma region
			//Check cueball
			bool cueBallCaught = false;
			for (int i = 0; i < CATCHER_COUNT && !cueBallCaught; i++)
			{
				//Calculate distance from catcher center to ball center
				float distance = sqrtf(pow(this->billiardCatchers[i].pos.x - this->activeBall.pos.x, 2) + pow(this->billiardCatchers[i].pos.z - this->activeBall.pos.z, 2));
				//Check if cueball is inside cather
				if (distance <= abs(this->billiardCatchers[i].radius - this->activeBall.radius))
				{
					//Cueball is inside catcher
					cueBallCaught = true;
				}
			}
			//Check other balls
			for (int ballIndex = 0; ballIndex < this->OTHER_BALL_COUNT; ballIndex++)
			{
				for (int catcherIndex = 0; catcherIndex < this->CATCHER_COUNT && this->ballState[ballIndex] > 0; catcherIndex++)
				{
					//Calculate distance from catcher center to ball center
					float distance = sqrtf(pow(this->billiardCatchers[catcherIndex].pos.x - this->otherBalls[ballIndex].pos.x, 2) + pow(this->billiardCatchers[catcherIndex].pos.z - this->otherBalls[ballIndex].pos.z, 2));
					//Check if ball is inside cather
					if (distance <= abs(this->billiardCatchers[catcherIndex].radius - this->otherBalls[ballIndex].radius))
					{
						//Set the ball state as CAUGHT = 0
						this->ballState[ballIndex] = 0;
					}
				}
			}
#pragma endregion Check balls within catchers
			//Check collisions with walls
			//WARNING this does not take cathers into account, need to change the algorithm to allow cathers
#pragma region
			//Check cueball
			if (this->activeBall.pos.x - this->activeBall.radius < -BOARD_HEIGHT / 2)
			{
				//Change direction
				DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f);
				this->activeBall.direction.x *= -1;
				//Fix collision
				this->activeBall.pos.x = -BOARD_HEIGHT / 2 + this->activeBall.radius;
				//Apply 5% velocity transfer to table
				this->activeBall.power *= 0.95f;
			}
			else if (this->activeBall.pos.x + this->activeBall.radius > BOARD_HEIGHT / 2)
			{
				//Change direction
				DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f);
				this->activeBall.direction.x *= -1;
				//Fix collision
				this->activeBall.pos.x = BOARD_HEIGHT / 2 - this->activeBall.radius;
				//Apply 5% velocity transfer to table
				this->activeBall.power *= 0.95f;
			}
			if (this->activeBall.pos.z - this->activeBall.radius < -BOARD_WIDTH / 2)
			{
				//Change direction
				DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
				this->activeBall.direction.z *= -1;
				//Fix collision
				this->activeBall.pos.z = -BOARD_WIDTH / 2 + this->activeBall.radius;
				//Apply 5% velocity transfer to table
				this->activeBall.power *= 0.95f;
			}
			else if (this->activeBall.pos.z + this->activeBall.radius > BOARD_WIDTH / 2)
			{
				//Change direction
				DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
				this->activeBall.direction.z *= -1;
				//Fix collision
				this->activeBall.pos.z = BOARD_WIDTH / 2 - this->activeBall.radius;
				//Apply 5% velocity transfer to table
				this->activeBall.power *= 0.95f;
			}

			//Check other balls
			for (int ballIndex = 0; ballIndex < OTHER_BALL_COUNT; ballIndex++)
			{
				//Don't update caught balls
				if (this->ballState[ballIndex] > 0)
				{
					if (this->otherBalls[ballIndex].pos.x - this->otherBalls[ballIndex].radius < -BOARD_HEIGHT / 2)
					{
						//Change direction
						DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f);
						this->otherBalls[ballIndex].direction.x *= -1;
						//Fix collision
						this->otherBalls[ballIndex].pos.x = -BOARD_HEIGHT / 2 + this->otherBalls[ballIndex].radius;
						//Apply 5% velocity transfer to table
						this->activeBall.power *= 0.95f;
					}
					else if (this->otherBalls[ballIndex].pos.x + this->otherBalls[ballIndex].radius > BOARD_HEIGHT / 2)
					{
						//Change direction
						DirectX::XMMatrixScaling(-1.0f, 1.0f, 1.0f);
						this->otherBalls[ballIndex].direction.x *= -1;
						//Fix collision
						this->otherBalls[ballIndex].pos.x = BOARD_HEIGHT / 2 - this->otherBalls[ballIndex].radius;
						//Apply 5% velocity transfer to table
						this->activeBall.power *= 0.95f;
					}
					if (this->otherBalls[ballIndex].pos.z - this->otherBalls[ballIndex].radius < -BOARD_WIDTH / 2)
					{
						//Change direction
						DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
						this->otherBalls[ballIndex].direction.z *= -1;
						//Fix collision
						this->otherBalls[ballIndex].pos.z = -BOARD_WIDTH / 2 + this->otherBalls[ballIndex].radius;
						//Apply 5% velocity transfer to table
						this->activeBall.power *= 0.95f;
					}
					else if (this->otherBalls[ballIndex].pos.z + this->otherBalls[ballIndex].radius > BOARD_WIDTH / 2)
					{
						//Change direction
						DirectX::XMMatrixScaling(1.0f, 1.0f, -1.0f);
						this->otherBalls[ballIndex].direction.z *= -1;
						//Fix collision
						this->otherBalls[ballIndex].pos.z = BOARD_WIDTH / 2 - this->otherBalls[ballIndex].radius;
						//Apply 5% velocity transfer to table
						this->activeBall.power *= 0.95f;
					}
				}
			}
#pragma endregion Check collisions with walls
			//Check collisions between balls
#pragma region
			//Go through all balls
			for (int i = 0; i < this->OTHER_BALL_COUNT; i++)
			{
				//If the ball is in play
				if (this->ballState[i] > 0)
				{
					DirectX::XMFLOAT2 distance = DirectX::XMFLOAT2(this->activeBall.pos.x - this->otherBalls[i].pos.x, this->activeBall.pos.z - this->otherBalls[i].pos.z);
					distance = DirectX::XMFLOAT2(this->otherBalls[i].pos.x - this->activeBall.pos.x, this->otherBalls[i].pos.z - this->activeBall.pos.z);

					//The physics calculation
					//if (sqrtf(pow(this->activeBall.pos.x - this->otherBalls[i].pos.x, 2) + pow(this->activeBall.pos.z - this->otherBalls[i].pos.z, 2)) < this->activeBall.radius + this->otherBalls[i].radius)
					//The sped up realtime calculation by removing the square root
					if (pow(distance.x, 2) + pow(distance.y, 2) < pow(this->activeBall.radius + this->otherBalls[i].radius, 2))
					{
						//Collision between active ball and other ball
						//Don't know what I will do with this information yet... Particle handler could use it though
						float collisionPointX = 0.0f, collisionPointZ = 0.0f;
						collisionPointX = ((this->otherBalls[i].pos.x * this->activeBall.radius) + (this->activeBall.pos.x * this->otherBalls[i].radius)) / (this->otherBalls[i].radius + this->activeBall.radius);
						collisionPointZ = ((this->otherBalls[i].pos.z * this->activeBall.radius) + (this->activeBall.pos.z * this->otherBalls[i].radius)) / (this->otherBalls[i].radius + this->activeBall.radius);
						//Move the balls back until they have just collided
						//Distance to move
						/*float overlapDistance = this->activeBall.radius + this->otherBalls[i].radius;
						overlapDistance -= sqrtf(pow(distance.x, 2) + pow(distance.y, 2));
						float collisionSpeed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->activeBall.direction))) - DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->otherBalls[i].direction)));
						float partOfCS = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->activeBall.direction))) / collisionSpeed;*/
						DirectX::XMVECTOR distanceVec = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(distance.x, 0.0f, distance.y));
						float overlapDistance = DirectX::XMVectorGetX(DirectX::XMVector3Length(distanceVec));
						DirectX::XMVECTOR mtd = DirectX::XMVectorScale(distanceVec, (this->activeBall.radius + this->otherBalls[i].radius - overlapDistance) / overlapDistance);
						//Translate them based on mass
						float inMass1 = 1 / this->otherBalls[i].mass, inMass2 = 1 / this->activeBall.mass;
						DirectX::XMFLOAT3 storeIn;
						DirectX::XMStoreFloat3(&storeIn, DirectX::XMVectorScale(mtd, inMass1 / (inMass1 + inMass2)));
						this->otherBalls[i].pos.x += storeIn.x;
						this->otherBalls[i].pos.z += storeIn.z;
						DirectX::XMStoreFloat3(&storeIn, DirectX::XMVectorScale(mtd, inMass2 / (inMass1 + inMass2)));
						this->activeBall.pos.x -= storeIn.x;
						this->activeBall.pos.z -= storeIn.z;

						



						bool stationary = false;
						if (this->activeBall.direction.x == 0.0f && this->activeBall.direction.z == 0.0f || this->otherBalls[i].direction.x == 0 && this->otherBalls[i].direction.z == 0)
						{
							stationary = true;
						}
						if (stationary || true)
						{
							DirectX::XMVECTOR un, ut;
							//Calculate the unit normal (un)
							un = DirectX::XMLoadFloat2(&distance);
							//un = DirectX::XMVectorScale(un, 1 / sqrtf(pow(distance.x, 2) + pow(distance.y, 2)));
							un = DirectX::XMVector2Normalize(un);
							//DirectX::XMVectorTan
							//Calculate the unit tangent (ut)
							//Manually
							ut = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(-DirectX::XMVectorGetY(un), DirectX::XMVectorGetX(un)));
							//By using DirectX
							ut = DirectX::XMVector2Orthogonal(un);
							//Define the vectors as 2d vectors
							DirectX::XMVECTOR v1, v2, v1n, v1t, v2n, v2t;
							DirectX::XMVECTOR newV1, newV2, newV1n, newV1t, newV2n, newV2t;
							v1 = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(this->otherBalls[i].direction.x, this->otherBalls[i].direction.z));
							v2 = DirectX::XMLoadFloat2(&DirectX::XMFLOAT2(this->activeBall.direction.x, this->activeBall.direction.z));
							v1n = DirectX::XMVector2Dot(un, v1);
							v1t = DirectX::XMVector2Dot(ut, v1);
							v2n = DirectX::XMVector2Dot(un, v2);
							v2t = DirectX::XMVector2Dot(ut, v2);
							//Calculate the post-collision vectors
							newV1t = v1t;
							newV2t = v2t;
							//Why am I using vectors for one dimensional calculations?
							newV1n = DirectX::XMVectorScale((DirectX::XMVectorAdd(DirectX::XMVectorScale(v1n, this->otherBalls[i].mass - this->activeBall.mass), DirectX::XMVectorScale(v2n, 2.0f * this->activeBall.mass))), 1.0f / (this->otherBalls[i].mass + this->activeBall.mass));
							newV2n = DirectX::XMVectorScale((DirectX::XMVectorAdd(DirectX::XMVectorScale(v2n, this->activeBall.mass - this->otherBalls[i].mass), DirectX::XMVectorScale(v1n, 2.0f * this->otherBalls[i].mass))), 1.0f / (this->otherBalls[i].mass + this->activeBall.mass));
							//Must be wrong
							/*newV1n = DirectX::XMVectorScale(newV1n, DirectX::XMVectorGetX(DirectX::XMVector2Length(un)));
							newV1t = DirectX::XMVectorScale(newV1t, DirectX::XMVectorGetX(DirectX::XMVector2Length(ut)));
							newV2n = DirectX::XMVectorScale(newV2n, DirectX::XMVectorGetX(DirectX::XMVector2Length(un)));
							newV2t = DirectX::XMVectorScale(newV2t, DirectX::XMVectorGetX(DirectX::XMVector2Length(ut)));
							newV1 = DirectX::XMVectorAdd(newV1n, newV1t);
							newV2 = DirectX::XMVectorAdd(newV2n, newV2t);*/
							//But the value in newV1(x) should be one dimensional right?
							newV1n = DirectX::XMVectorMultiply(newV1n, un);
							newV1t = DirectX::XMVectorMultiply(newV1t, ut);
							newV2n = DirectX::XMVectorMultiply(newV2n, un);
							newV2t = DirectX::XMVectorMultiply(newV2t, ut);
							newV1 = DirectX::XMVectorAdd(newV1n, newV1t);
							newV2 = DirectX::XMVectorAdd(newV2n, newV2t);
							DirectX::XMStoreFloat3(&otherBalls[i].direction, newV1);
							this->otherBalls[i].direction.z = this->otherBalls[i].direction.y;
							this->otherBalls[i].direction.y = 0.0f;
							DirectX::XMStoreFloat3(&this->activeBall.direction, newV2);
							this->activeBall.direction.z = this->activeBall.direction.y;
							this->activeBall.direction.y = 0.0f;
						}
					}
					for (int j = i + 1; j < this->OTHER_BALL_COUNT; j++)
					{
						if (this->ballState[j] > 0)
						{
							distance = DirectX::XMFLOAT2(this->otherBalls[j].pos.x - this->otherBalls[i].pos.x, this->otherBalls[j].pos.z - this->otherBalls[i].pos.z);
							//Between other balls
							if (pow(this->otherBalls[j].pos.x - this->otherBalls[i].pos.x, 2) + pow(this->otherBalls[j].pos.z - this->otherBalls[i].pos.z, 2) < pow(this->otherBalls[j].radius + this->otherBalls[i].radius, 2))
							{
								//Collision between other balls
								float collisionPointX = 0.0f, collisionPointZ = 0.0f;
								collisionPointX = ((this->otherBalls[i].pos.x * this->otherBalls[j].radius) + (this->otherBalls[j].pos.x * this->otherBalls[i].radius)) / (this->otherBalls[i].radius + this->otherBalls[j].radius);
								collisionPointZ = ((this->otherBalls[i].pos.z * this->otherBalls[j].radius) + (this->otherBalls[j].pos.z * this->otherBalls[i].radius)) / (this->otherBalls[i].radius + this->otherBalls[j].radius);
								//Check so that the balls are not moving away from eachother...
								DirectX::XMFLOAT3 dotProduct;
								DirectX::XMStoreFloat3(&dotProduct, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&this->otherBalls[i].direction), DirectX::XMLoadFloat3(&this->otherBalls[j].direction)));
								if (dotProduct.x > 0 || dotProduct.y > 0 || dotProduct.z > 0)
								{

								}
							}
						}
					}
				}
			}
#pragma endregion Check collisions between balls
			//Apply friction
#pragma region
			//Calculate normalforce
			float fN = GRAVITY * this->activeBall.mass;
			float fF = fN * FRICTIONCOEFFICIENT_BETWEEN_BALL_AND_VELVET;
			//Calculate the friction vector (for applying the friction)
			DirectX::XMFLOAT3 fVec;
			DirectX::XMStoreFloat3(&fVec, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&this->activeBall.direction)));
			fVec.y = 0.0f;
			fVec.x = -fVec.x * fF * deltaTime;
			fVec.z = -fVec.z * fF * deltaTime;
			//Calculate the length of the friction vector
			float friktionPower = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&fVec)));
			//If the friktion power is higher than the remaining speed, the object should no longer be able to move
			//Another solution would be to set the friction to be equal to the strength of the direction so that the result is 0.0f
			//but it will not be implemented that way because of float error might result in a very slight remaining part
			if (friktionPower < DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->activeBall.direction))))
			{
				this->activeBall.direction.x += fVec.x;
				this->activeBall.direction.z += fVec.z;
			}
			else
			{
				this->activeBall.direction.x = 0.0f;
				this->activeBall.direction.z = 0.0f;
			}

			for (int i = 0; i < OTHER_BALL_COUNT; i++)
			{
				fN = GRAVITY * this->otherBalls[i].mass;
				fF = fN * FRICTIONCOEFFICIENT_BETWEEN_BALL_AND_VELVET;
				DirectX::XMStoreFloat3(&fVec, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&this->otherBalls[i].direction)));
				fVec.y = 0.0f;
				fVec.x = -fVec.x * fF * deltaTime;
				fVec.z = -fVec.z * fF * deltaTime;
				float friktionPower = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&fVec)));
				/*if (friktionPower < DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->activeBall.direction))))
				{
					this->otherBalls[i].direction.x += fVec.x;
					this->otherBalls[i].direction.z += fVec.z;
				}
				else
				{
					this->otherBalls[i].direction.x = 0.0f;
					this->otherBalls[i].direction.z = 0.0f;
				}*/
				this->otherBalls[i].direction.x += fVec.x;
				this->otherBalls[i].direction.z += fVec.z;
			}
#pragma endregion friction
			//Check if no balls are moving.
			bool ballMoving = false;
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->activeBall.direction))) > NOT_MOVING_FLOAT)
			{
				ballMoving = true;
			}
			else
			{
				this->activeBall.direction.x = 0.0f;
				this->activeBall.direction.y = 0.0f;
				this->activeBall.direction.z = 0.0f;
			}
			for (int i = 0; i < OTHER_BALL_COUNT && !ballMoving; i++)
			{
				float directionSpeed = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&this->otherBalls[i].direction)));
				if ( directionSpeed > NOT_MOVING_FLOAT)
				{
					ballMoving = true;
				}
				else
				{
					this->otherBalls[i].direction.x = 0.0f;
					this->otherBalls[i].direction.y = 0.0f;
					this->otherBalls[i].direction.z = 0.0f;
				}
			}
			//ballMoving = true;
			if (ballMoving == false)
			{
				int apdw = 0.0f;
			}
			this->simulationCompleted = !ballMoving;

			//Set the positions of the ball models to be accurate
			DirectX::XMMATRIX posOffset;
			posOffset = DirectX::XMMatrixTranslation(this->activeBall.pos.x / SCALING, this->activeBall.pos.y / SCALING, this->activeBall.pos.z / SCALING);
			posOffset = DirectX::XMMatrixMultiply(posOffset, DirectX::XMMatrixScaling(SCALING, SCALING, SCALING));
			this->m_cueBall.SetWorldMatrix(posOffset);
			posOffset = DirectX::XMMatrixTranslation(this->otherBalls[0].pos.x / SCALING, this->otherBalls[0].pos.y / SCALING, this->otherBalls[0].pos.z / SCALING);
			posOffset = DirectX::XMMatrixMultiply(posOffset, DirectX::XMMatrixScaling(SCALING, SCALING, SCALING));
			this->m_8Ball.SetWorldMatrix(posOffset);
		}
		else
		{
			//If the balls are all stationary, prepare for another shot from the active ball
			//Check for a mouse right click
			if (input->isMouseKeyReleased(0))
			{
				//Check the mouse position compared to the ball center
				float xPos = 0.0f, yPos = 0.0f;
				//Use that data to calculate a direction matrix to use for the ball
				//Set the power of the ball
				//Start simulating ball movement again
				this->simulationCompleted = false;
			}
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
