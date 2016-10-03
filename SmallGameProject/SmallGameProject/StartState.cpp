#include "StartState.h"
#include "GameStateHandler.h"


StartState::StartState()
{
	this->startGame = false;
	this->manualClearing = false;
	this->startModel = Model();
	this->camera = CameraHandler();
}

StartState::StartState(GameStateHandler * GSH)
{
	this->m_GSH = GSH;
}

StartState::~StartState()
{
}

void StartState::Shutdown()
{
	GameState::Shutdown();
	
	this->startModel.Shutdown();
}

int StartState::Initialize(GraphicHandler* gHandler, GameStateHandler* GSH)
{
	int result = 0;
	this->startModel = Model();
	this->manualClearing = false;
	//Initialize the base class GameState

	ID3D11Device* device = gHandler->GetDevice();
	ID3D11DeviceContext* deviceContext = gHandler->GetDeviceContext();

	result = this->InitializeBase(GSH, device, deviceContext);
	if (result)
	{
		//Proceed to initialize thyself
		bool modelResult = this->startModel.Initialize(device, deviceContext, "StartGame");
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 40.0f);
		worldMatrix *= DirectX::XMMatrixScaling(0.02f, 0.02f, 0.02f);
		worldMatrix = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2) * worldMatrix;
		modelResult = this->camera.Initialize();
		this->startModel.SetWorldMatrix(worldMatrix);

		PointLight light;
		light.Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Position = DirectX::XMFLOAT4(0.0f, 0.0f, -4.0f, 1.0f);
		light.Attenuation = DirectX::XMFLOAT4(50.0f, 1.0f, 0.09f, 0.032f);

		this->pointLights.push_back(light);
	}

	return result;
}

int StartState::HandleInput(InputHandler * input)
{
	int result = 0;
	if (input->isKeyPressed(DIK_SPACE))
	{
		this->startGame = true;
	}
	return result;
}

int StartState::Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler)
{
	int result = 1;
	if (this->startGame)
	{
		this->startGame = false;
		//Create the menu and push it
 		MenuState* menu = new MenuState();
		menu->Initialize(gHandler, this->m_GSH);
		menu->SetManualClearing(false);
		this->m_GSH->PushState(menu);
	}
	return result;
}

int StartState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	int result = 0;

	gHandler->DeferredRender(&this->startModel, &this->camera);

	gHandler->LightRender(this->camera.GetCameraPos(), this->pointLights);
	
	return result;
}
