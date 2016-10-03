#ifndef SOUNDHANDLER_H
#define SOUNDHANDLER_H

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <vector>
#include <string>

const int NUMBER_OF_SOUNDS = 3;

class SoundHandler
{

private:

	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};
	struct WaveHeaderType2
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[38];
		unsigned long dataSize;
	};


	IDirectSound8* directSound;
	IDirectSoundBuffer* primaryBuffer;
	std::vector<IDirectSoundBuffer8*> secondaryBuffers;	//Need one secoundary buffer for each sound
	std::string fileNames[NUMBER_OF_SOUNDS];

	bool InitializeDirectSound(HWND hwnd);

	bool LoadWaveFile(char* filePath, IDirectSoundBuffer8** aSecoundaryBuffer);
	void ShutdownWaveFile(IDirectSoundBuffer8**aSecondaryBuffer);

public:
	SoundHandler();
	~SoundHandler(); 
	
	bool Initialize(HWND hwnd);
	void Shutdown();

	bool PlayWaveFile(char* fileName);

};

#endif

