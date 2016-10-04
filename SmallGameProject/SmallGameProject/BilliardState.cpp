#include "BilliardState.h"



BilliardState::BilliardState()
{
	this->myCamera = CameraHandler();

	this->m_cueBall = Model();
	this->m_8Ball = Model();
	this->m_cueStick = Model();
	this->m_table = Model();

	this->simulationCompleted = true;
	

}


BilliardState::~BilliardState()
{
}

void BilliardState::Shutdown()
{
	GameState::Shutdown();
	this->m_cueBall.Shutdown();
	this->m_8Ball.Shutdown();
	this->m_cueStick.Shutdown();
	this->m_table.Shutdown();
}

int BilliardState::Initialize(GraphicHandler * gHandler, GameStateHandler * GSH)
{
	int result = 0;
	this->simulationCompleted = true;

	this->billiardCatchers[0].radius = 6.5f;
	this->billiardCatchers[1].radius = 6.5f;
	this->billiardCatchers[2].radius = 10.0f;
	this->billiardCatchers[3].radius = 10.0f;
	this->billiardCatchers[4].radius = 10.0f;
	this->billiardCatchers[5].radius = 10.0f;

	this->billiardCatchers[0].pos.x = -72.300f;
	this->billiardCatchers[1].pos.x = 72.300f;
	this->billiardCatchers[2].pos.x = -70.0f;
	this->billiardCatchers[3].pos.x = 70.0f;
	this->billiardCatchers[4].pos.x = -70.0f;
	this->billiardCatchers[5].pos.x = 70.0f;
	this->billiardCatchers[0].pos.z = 0.0f;
	this->billiardCatchers[1].pos.z = 0.0f;
	this->billiardCatchers[2].pos.z = -133.0f;
	this->billiardCatchers[3].pos.z = -133.0f;
	this->billiardCatchers[4].pos.z = 133.0f;
	this->billiardCatchers[5].pos.z = 133.0f;

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

		modelResult = this->m_table.Initialize(device, this->m_deviceContext, "Shuttle");
		if (!modelResult) {
			return -1;
		}
		//Secondly thy must lift why mighty sword of obj! (move the objects around)
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixScaling(SCALING, SCALING, SCALING);
		this->m_table.SetWorldMatrix(worldMatrix);


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

		//And the developer said "Let there be light!", and so light came to be
		PointLight light;
		light.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(0.0f, 0.0f, -10.0f, 1.0f);
		light.Attenuation = DirectX::XMFLOAT4(50.0f, 1.0f, 0.09f, 0.032f);
		this->pointLights.push_back(light);

	}

	return result;
}

int BilliardState::HandleInput(InputHandler * input)
{
	return 0;
}

int BilliardState::Update(float deltaTime, InputHandler * input, GraphicHandler * gHandler)
{
	return 0;
}

int BilliardState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	return 0;
}
