#ifndef SYSTEM_H
#define SYSTEM_H

#include <Windows.h>
#include <chrono>
#include "GameStateHandler.h"
#include "GraphicHandler.h"
#include "InputHandler.h"
#include "Model.h"
#include "CameraHandler.h"
#include "GameData.h"

const bool FULL_SCREEN = false;

class System {

private:
	HWND hwnd;
	HINSTANCE hinstance;
	LPCWSTR applicationName;

	GameStateHandler* gameSH;
	GraphicHandler* graphicH;
	InputHandler* inputH;
	CameraHandler* cameraH;
	UIHandler uiHandler;

	//gameData singelton, keeps track of all the game progression
	GameData* gameData;

	void InitWindow(int& screenWidth, int& screenHeight);
	void ShutdownWindow();
	bool Update(float dTime);


	float testRot;
public:
	System();
	~System();

	bool Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT unmessage, WPARAM wparam, LPARAM lparam);
static System* ApplicationHandle = nullptr;

#endif
