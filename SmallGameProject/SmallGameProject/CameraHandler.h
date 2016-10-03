#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <DirectXMath.h>

class CameraHandler {
private:
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX baseViewMatrix;
	DirectX::XMVECTOR cameraPos;
	DirectX::XMVECTOR lookAt;
	DirectX::XMVECTOR cameraUp;

	float roll;
	float pitch;
	float yaw;

public:
	CameraHandler();
	~CameraHandler();

	bool Initialize();
	void GetBaseViewMatrix(DirectX::XMMATRIX& baseViewMatrix);
	void GetViewMatrix(DirectX::XMMATRIX& viewMatrix);

	DirectX::XMFLOAT4 GetCameraPos();
	void SetCameraPos(DirectX::XMFLOAT3 newCamPos);

	DirectX::XMFLOAT3 GetLookAt();
	void SetLookAt(DirectX::XMFLOAT3 newLookAt);

	DirectX::XMFLOAT3 GetCameraUp();
	void SetCameraUp(DirectX::XMFLOAT3 newCamUp);

	float GetRoll();
	void SetRoll(float newRoll, bool add = false);

	float GetPitch();
	void SetPitch(float newPitch, bool add = false);

	float GetYaw();
	void SetYaw(float newYaw, bool add = false);

	void UpdateCamera();
};

#endif