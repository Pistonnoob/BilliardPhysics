#include "SoundHandler.h"


SoundHandler::SoundHandler()
{
	this->directSound = nullptr;
	this->primaryBuffer = nullptr;
	
	for (int i = 0; i < NUMBER_OF_SOUNDS; i++) {
		this->fileNames[i] = "";
	}
}

SoundHandler::~SoundHandler()
{
}

void SoundHandler::Shutdown()
{
	// Release the secondary buffers.
	for (auto secoundaryBuffer : this->secondaryBuffers) {
		ShutdownWaveFile(&secoundaryBuffer);
	}

	this->secondaryBuffers.clear();

	// Release the primary sound buffer pointer.
	if (primaryBuffer)
	{
		primaryBuffer->Release();
		primaryBuffer = nullptr;
	}

	// Release the direct sound interface pointer.
	if (directSound)
	{
		directSound->Release();
		directSound = nullptr;
	}

	return;
}

bool SoundHandler::Initialize(HWND hwnd)
{
	bool result;

	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	for (int i = 0; i < NUMBER_OF_SOUNDS; i++) {
		this->secondaryBuffers.push_back(nullptr);
	}

	// Load the wave audio files onto a secondary buffer. (Onces for each)
	result = LoadWaveFile("..\\SmallGameProject\\Resources\\Sounds\\click.wav", &this->secondaryBuffers.at(0));
	this->fileNames[0] = "click";
	if (!result)
	{
		return false;
	}
	
	result = LoadWaveFile("..\\SmallGameProject\\Resources\\Sounds\\gun.wav", &this->secondaryBuffers.at(1));
	this->fileNames[1] = "gun";
	if (!result)
	{
		return false;
	}
	result = LoadWaveFile("..\\SmallGameProject\\Resources\\Sounds\\explosion.wav", &this->secondaryBuffers.at(2));
	this->fileNames[2] = "explosion";
	if (!result)
	{
		return false;
	}


	return true;
}

bool SoundHandler::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &directSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool SoundHandler::LoadWaveFile(char * filePath, IDirectSoundBuffer8 **secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WaveHeaderType2 waveFileHeader2;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char *bufferPtr;
	unsigned long bufferSize;
	bool otherType = false;

	// Open the wave file in binary.
	error = fopen_s(&filePtr, filePath, "rb");
 	if (error != 0)
	{
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		fseek(filePtr, 0, SEEK_SET);

		// Read in the wave file header.
		count = fread(&waveFileHeader2, sizeof(waveFileHeader2), 1, filePtr);
		otherType = true;
		if (count != 1)
		{
			return false;
		}
		

	}


	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	
	if (!otherType) {
		bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	}
	else {
		bufferDesc.dwBufferBytes = waveFileHeader2.dataSize;
	}
	
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = nullptr;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	if (!otherType) {
		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
		waveData = new unsigned char[waveFileHeader.dataSize];
	}
	else {
		fseek(filePtr, sizeof(WaveHeaderType2), SEEK_SET);
		waveData = new unsigned char[waveFileHeader2.dataSize];
	}
	
	// Create a temporary buffer to hold the wave file data.
	
	if (!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	if (!otherType) {
		count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
		if (count != waveFileHeader.dataSize)
		{
			//return false;
		}
	}
	else {
		count = fread(waveData, 1, waveFileHeader2.dataSize, filePtr);
		if (count != waveFileHeader2.dataSize)
		{
			//return false;
		}
	}
	
	

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	if (!otherType) {
		result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	}
	else {
		result = (*secondaryBuffer)->Lock(0, waveFileHeader2.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	}
	
	if (FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	if (!otherType) {
		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
	}
	else {
		memcpy(bufferPtr, waveData, waveFileHeader2.dataSize);
	}


	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = nullptr;

	return true;
}

void SoundHandler::ShutdownWaveFile(IDirectSoundBuffer8 ** secondaryBuffer)
{
	// Release the secondary sound buffer.	(Once for each sound secoundary buffer)
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = nullptr;
	}

	return;
}

bool SoundHandler::PlayWaveFile(char* fileName)
{
	HRESULT result;

	for (int i = 0; i < NUMBER_OF_SOUNDS; i++) {

		//Check if we have loaded a sound file with the same file name
		if (fileName == this->fileNames[i]) {

			//Try to play the sound
			IDirectSoundBuffer8* secondaryBuffer = this->secondaryBuffers.at(i);

			// Set position at the beginning of the sound buffer.
			result = secondaryBuffer->SetCurrentPosition(0);
			if (FAILED(result))
			{
				return false;
			}

			// Set volume of the buffer to 100%.
			result = secondaryBuffer->SetVolume(DSBVOLUME_MAX);
			if (FAILED(result))
			{
				return false;
			}

			// Play the contents of the secondary sound buffer.
			result = secondaryBuffer->Play(0, 0, 0);
			if (FAILED(result))
			{
				return false;
			}

			return true;	//If we managed to play the sound
		}

	}

	return false;	//If there was matching file name
}


