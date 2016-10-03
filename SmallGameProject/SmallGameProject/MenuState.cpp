#include "MenuState.h"
#include "GameStateHandler.h"
#include "Algorithm.h"


MenuState::MenuState()
{
	this->first = true;
	this->doOption = false;
	this->selected = 0;
	this->m_model = Model();
	this->myTextures = Texture();
	this->myCamera = CameraHandler();
}


MenuState::~MenuState()
{
}

void MenuState::Shutdown()
{
	GameState::Shutdown();
	this->myTextures.Shutdown();
	this->m_model.Shutdown();
}

int MenuState::Initialize(GraphicHandler* gHandler, GameStateHandler * GSH)
{
	int result = 0;
	this->first = true;
	this->doOption = false;
	this->selected = 0;
	this->m_model = Model();

	ID3D11Device* device = gHandler->GetDevice();
	ID3D11DeviceContext* deviceContext = gHandler->GetDeviceContext();

	//Initialize the base class GameState
	result = this->InitializeBase(GSH, device, deviceContext);
	if (result)
	{
		//Proceed to initialize thyself
		//Firstly thy must initialize thy mighty sword of obj!
		bool victory = m_model.Initialize(device, deviceContext, "Menu2D");
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
		worldMatrix = DirectX::XMMatrixTranslation(0.0f, -5.2f, 0.0f);
		worldMatrix = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2) * worldMatrix;
		this->myCamera.SetCameraPos(DirectX::XMFLOAT3(0.0f, 0.0f, -20.0f));
		victory = this->myCamera.Initialize();
		this->m_model.SetWorldMatrix(worldMatrix);

		//Now initialize thy shields (texture views of changing nature!)
		this->myTextures = Texture();
		std::string skinOfMyFallenEnemies = "MenuSelected.mtl";
		victory = this->myTextures.Initialize(device, deviceContext, skinOfMyFallenEnemies);

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

int MenuState::HandleInput(InputHandler * input)
{
	int result = 1;
	int oldSelected = this->selected;
	if (input->isKeyPressed(DIK_SPACE) || input->isKeyPressed(DIK_RETURN))
	{
		this->doOption = true;
	}
	if (input->isMouseKeyPressed(0))
	{
		this->doOption = true;
	}
	if (input->isKeyPressed(DIK_DOWNARROW))
	{
		this->selected++;
	}
	if (input->isKeyPressed(DIK_UPARROW))
	{
		this->selected--;
	}

	if (!this->first) {
		DirectX::XMFLOAT2 mousePos = input->getMousePosInWindow(); //Top left 220, 11; bottom right 580, 200

		if (mousePos.x > 220 && mousePos.x < 580) {
			if (mousePos.y > 10) {
				if (mousePos.y < 200) {
					this->selected = 0;
				}
				else if (mousePos.y < 390) {
					this->selected = 1;
				}
				else if (mousePos.y < 580) {
					this->selected = 2;
				}
			}
		}
	}

	if (this->selected < 0)
		this->selected = OPTION_COUNT - 1;
	this->selected = this->selected % OPTION_COUNT;
	//If the selection changed
	if (oldSelected != this->selected)
	{
		//Switch the textures
		ID3D11ShaderResourceView* oscarHasFallen = this->m_model.SwapTextureView(this->myTextures.GetTexture(this->selected), this->selected);
		this->myTextures.SwapTextureView(oscarHasFallen, this->selected);
		ID3D11ShaderResourceView* axelHasFallen = this->m_model.SwapTextureView(this->myTextures.GetTexture(oldSelected), oldSelected);
		this->myTextures.SwapTextureView(axelHasFallen, oldSelected);
	}
	else if (this->first)
	{
		this->first = false;
 		ID3D11ShaderResourceView* oscarHasFallen = this->m_model.SwapTextureView(this->myTextures.GetTexture(this->selected), this->selected);
		this->myTextures.SwapTextureView(oscarHasFallen, this->selected);
	}

	if (input->isKeyPressed(DIK_ESCAPE))
	{
		//Ask Loki to cheat and lie to the system that we have used the option END_GAME
		this->doOption = true;
		this->selected = MenuOption::END_GAME;
	}

	return result;
}

int MenuState::Update(float deltaTime, InputHandler* input, GraphicHandler* gHandler)
{
	int result = 1;

	if (doOption)
	{
		this->doOption = false;
		switch (this->selected)
		{
		case MenuOption::START_GAME:
		{
			//Create a stage state and push it to the stack
			HubState* newStage = new HubState();
			newStage->Initialize(gHandler, this->m_GSH);
			newStage->SetManualClearing(false);
			this->m_GSH->PushState(newStage);
		}
		break;
		case MenuOption::OPTIONS:
			break;
		case MenuOption::END_GAME:
		{
			result = 0;
			break;
		}
		default:
			break;
		}
	}

	return result;
}

int MenuState::Render(GraphicHandler * gHandler, HWND hwnd)
{
	int result = 0;

	gHandler->DeferredRender(&this->m_model, &this->myCamera);

	gHandler->LightRender(this->myCamera.GetCameraPos(), this->pointLights);

	return result;
}
