#include "InputHandler.h"

InputHandler::InputHandler()
{
	this->DIKeyboard = nullptr;
	this->DIMouse = nullptr;
	this->mouseX = 0;
	this->mouseY = 0;
	this->screenWidth = 0;
	this->screenWidth = 0;
	this->lastKeyPressed = -1;

	for (int i = 0; i < 256; i++) {
		this->KeyboarState[i] = this->OldKeyboardState[i] = false;
	}

}

InputHandler::~InputHandler()
{
	this->Shutdown();
}

void InputHandler::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT hr;
	//Mouse initial starting position set to the midle of the screen
	SetPhysicalCursorPos((GetSystemMetrics(SM_CXSCREEN)) / 2, (GetSystemMetrics(SM_CYSCREEN)) / 2);
	this->mouseX = (GetSystemMetrics(SM_CXSCREEN)) / 2;
	this->mouseY = (GetSystemMetrics(SM_CYSCREEN)) / 2;
	//Save the resolution for future use
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	//Initialize the DirectInput Object
	hr = DirectInput8Create(
		hInstance,					// Instance handle to the application
		DIRECTINPUT_VERSION,		//	dwVersion
		IID_IDirectInput8,			//	RefiID, Unique identifier of the desired interface
		(void**)&this->directInput,	//	The pointer who will recive the object
		NULL);						//	LPUNKNOWN punkOuter

									//Check if the object has been created
	if (FAILED(hr)) {
		MessageBox(hwnd, L"DirectInput8Create", L"Error", MB_OK);
	}

	//Create the keyboard device
	hr = this->directInput->CreateDevice(
		GUID_SysKeyboard,					// What type of device
		&this->DIKeyboard,					// Pointer who will recive the object
		NULL);								//	LPUNKNOWN punkOuter

											//Check if the keyboard object has been created
	if (SUCCEEDED(hr)) {

		//Set the options for the keyboard
		hr = this->DIKeyboard->SetDataFormat(&c_dfDIKeyboard);	// Data format of the device

		if (FAILED(hr)) {
			MessageBox(hwnd, L"SetDataFormat_Keyboard", L"Error", MB_OK);
		}

		hr = this->DIKeyboard->SetCooperativeLevel(
			hwnd,
			DISCL_NONEXCLUSIVE | DISCL_FOREGROUND
			);

		if (FAILED(hr)) {
			MessageBox(hwnd, L"SetCooperativeLevel_Keyboard", L"Error", MB_OK);
		}

		hr = this->DIKeyboard->Acquire();	//Aquire the keyboard

		if (FAILED(hr)) {
			MessageBox(hwnd, L"Acquire_Keyboard", L"Error", MB_OK);
		}

	}
	else {
		MessageBox(hwnd, L"CreateDevice_Keyboard", L"Error", MB_OK);
	}

	//Create the Mouse mouse device
	hr = this->directInput->CreateDevice(
		GUID_SysMouse,
		&this->DIMouse,
		NULL);

	//Check if the mouse object has been created
	if (SUCCEEDED(hr)) {

		//Set the options for the mouse
		hr = this->DIMouse->SetDataFormat(&c_dfDIMouse);	// Data format of the device

		if (FAILED(hr)) {
			MessageBox(hwnd, L"SetDataFormat_Mouse", L"Error", MB_OK);
		}

		hr = this->DIMouse->SetCooperativeLevel(
			hwnd,
			DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND
			);

		if (FAILED(hr)) {
			MessageBox(hwnd, L"SetCooperativeLevel_Mouse", L"Error", MB_OK);
		}

		hr = this->DIMouse->Acquire();	//Aquire the mouse

		if (FAILED(hr)) {
			MessageBox(hwnd, L"Acquire_Mouse", L"Error", MB_OK);
		}

	}
	else {
		MessageBox(hwnd, L"CreateDevice_Mouse", L"Error", MB_OK);
	}

	return;
}

void InputHandler::Shutdown()
{
	if (this->DIKeyboard) {
		this->DIKeyboard->Unacquire();
		this->DIKeyboard->Release();
		this->DIKeyboard = nullptr;
	}

	if (this->DIMouse) {
		this->DIMouse->Unacquire();
		this->DIMouse->Release();
		this->DIMouse = nullptr;
	}

	if (this->directInput) {
		this->directInput->Release();
	}
}

void InputHandler::Update()
{

	//Check if we can read the devices. 
	//If we cant, the old data will be used
	
	this->ReadKeyboard();
	this->ReadMouse();

	this->ProcessInput();

}

void InputHandler::ReadKeyboard()
{
	HRESULT hr;
	//Copy the old data
	for (int i = 0; i < 256; i++) {
		this->OldKeyboardState[i] = this->KeyboarState[i];
	}
	//Read the keyboard device
	hr = this->DIKeyboard->GetDeviceState(
		sizeof(this->KeyboarState),
		(LPVOID)&this->KeyboarState);

	if (FAILED(hr)) {

		//If the keyboard lost focus or was not acquired, try to get control back
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			this->DIKeyboard->Acquire();
		}

	}

	return;
}

void InputHandler::ReadMouse()
{
	HRESULT hr;
	this->OldDIMouseState = this->DIMouseState;

	//Read the mouse device
	hr = this->DIMouse->GetDeviceState(
		sizeof(DIMOUSESTATE),
		&this->DIMouseState);

	if (FAILED(hr)) {

		//If the mouse lost focus or was not acquired, try to get control back
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			this->DIMouse->Acquire();
		}

	}


	return;
}

void InputHandler::ProcessInput()
{

	this->mouseX += this->DIMouseState.lX;
	this->mouseY += this->DIMouseState.lY;

	//Update the mouse position (not physicly)

	//if (this->mouseX < 0) {
	//	this->mouseX = 0;
	//}

	//if (this->mouseX > this->screenWidth) {
	//	this->mouseX = this->screenWidth;
	//}

	//if (this->mouseY < 0) {
	//	this->mouseY = 0;
	//}

	//if (this->mouseY > this->screenHeight) {
	//	this->mouseY = this->screenHeight;
	//}
}

DirectX::XMVECTOR InputHandler::GetMouseDeltaPos()
{
	return DirectX::XMVectorSet(this->DIMouseState.lX, this->DIMouseState.lY, 0, 0);	//z,y is not used so set to 0
}

void InputHandler::KeyDown(unsigned int key)
{
	this->KeyboarState[key] = true;

	return;
}

void InputHandler::KeyUp(unsigned int key)
{
	this->KeyboarState[key] = false;
	this->lastKeyPressed = key;

	return;
}

bool InputHandler::isKeyDown(unsigned int key)
{
	if (this->KeyboarState[key]) {
		return true;
	}

	return false;
}

bool InputHandler::isKeyPressed(unsigned int key)
{
	if (!this->OldKeyboardState[key] && this->KeyboarState[key]) {
		return true;
	}

	return false;
}

bool InputHandler::isKeyReleased(unsigned int key)
{
	if (this->OldKeyboardState[key] && !this->KeyboarState[key]) {
		return true;
	}

	return false;
}

bool InputHandler::isMouseKeyPressed(unsigned int key)
{
	if (!this->OldDIMouseState.rgbButtons[key] && DIMouseState.rgbButtons[key]) {
		return true;
	}

	return false;
}

bool InputHandler::isMouseKeyDown(unsigned int key)
{
	if (this->DIMouseState.rgbButtons[key]) {
		return true;
	}

	return false;
}

bool InputHandler::isMouseKeyReleased(unsigned int key)
{
	if (this->OldDIMouseState.rgbButtons[key] && !this->DIMouseState.rgbButtons[key]) {
		return true;
	}

	return false;
}

DirectX::XMFLOAT2 InputHandler::getMousePos()
{
	POINT tempP;
	GetCursorPos(&tempP);

	return DirectX::XMFLOAT2(tempP.x, tempP.y);;
}

DirectX::XMFLOAT2 InputHandler::getMousePosInWindow()
{
	DirectX::XMFLOAT2 mousePos = this->getMousePos();
	//Move the cords to the window
	float mouseX = mousePos.x - (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	float mouseY = mousePos.y - (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	return DirectX::XMFLOAT2(mouseX, mouseY);
}
