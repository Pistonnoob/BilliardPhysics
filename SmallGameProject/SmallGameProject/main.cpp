//FIRST!!

//#include <windows.h>
#include "System.h"
#include <stdio.h>
#include <crtdbg.h>
#include "DXGIDebug.h"
#include <ctime>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(unsigned(time(NULL)));

	//MessageBox(NULL, L"TestBox", L"TestBox", MB_ICONEXCLAMATION | MB_OK);
	System* sH;
	bool result;

	//Create the system object
	sH = new System;

	if (!sH) {
		return 0;
	}

	//Initialize and run the system object
	result = sH->Initialize();
	if (result) {
		sH->Run();
	}

	//Shutdown and release the system object
	sH->Shutdown();
	delete sH;
	sH = nullptr;
	return 0;
}