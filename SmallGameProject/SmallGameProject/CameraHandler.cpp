#include "CameraHandler.h"

#include <windows.h>
#include <Windows.h>

CameraHandler::CameraHandler()
{
	this->viewMatrix = DirectX::XMMatrixIdentity();
	this->cameraPos = DirectX::XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f);
	this->lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	this->cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	this->roll = 0.0f;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
}
CameraHandler::~CameraHandler()
{

}

bool CameraHandler::Initialize()
{
	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR camPos = DirectX::XMVectorSet(0.0f, 0.0f, -20.0f, 1.0f);
	DirectX::XMVECTOR camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	
	this->baseViewMatrix = DirectX::XMMatrixLookAtLH(camPos, lookAt, camUp);

	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->pitch, this->yaw, this->roll);

	camPos = DirectX::XMVector3TransformCoord(this->cameraPos, camRotationMatrix);
	lookAt = DirectX::XMVector3TransformCoord(this->lookAt, camRotationMatrix);
	camUp = DirectX::XMVector3TransformCoord(this->cameraUp, camRotationMatrix);

	this->viewMatrix = DirectX::XMMatrixLookAtLH(this->cameraPos, this->lookAt, this->cameraUp);

	return true;
}

void CameraHandler::GetBaseViewMatrix(DirectX::XMMATRIX & baseViewMatrix)
{
	baseViewMatrix = this->baseViewMatrix;
}

void CameraHandler::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
}
DirectX::XMFLOAT4 CameraHandler::GetCameraPos()
{
	DirectX::XMFLOAT4 camPosFloat;

	DirectX::XMStoreFloat4(&camPosFloat, this->cameraPos);

	return camPosFloat;
}

void CameraHandler::SetCameraPos(DirectX::XMFLOAT3 newCamPos)
{
	this->cameraPos = DirectX::XMLoadFloat3(&newCamPos);
}

DirectX::XMFLOAT3 CameraHandler::GetLookAt()
{
	DirectX::XMFLOAT3 lookAtFloat;

	DirectX::XMStoreFloat3(&lookAtFloat, this->lookAt);

	return lookAtFloat;
}

void CameraHandler::SetLookAt(DirectX::XMFLOAT3 newLookAt)
{
	this->lookAt = DirectX::XMLoadFloat3(&newLookAt);
}

DirectX::XMFLOAT3 CameraHandler::GetCameraUp()
{
	DirectX::XMFLOAT3 camUpFloat;

	DirectX::XMStoreFloat3(&camUpFloat, this->cameraUp);

	return camUpFloat;
}

void CameraHandler::SetCameraUp(DirectX::XMFLOAT3 newCamUp)
{
	this->cameraUp = DirectX::XMLoadFloat3(&newCamUp);
}

float CameraHandler::GetRoll()
{
	return this->roll;
}

void CameraHandler::SetRoll(float newRoll, bool add)
{
	if (!add) {
		this->roll = newRoll;
	}
	else {
		this->roll += newRoll;
	}
}

float CameraHandler::GetPitch()
{
	return this->pitch;
}

void CameraHandler::SetPitch(float newPitch, bool add)
{
	if (!add) {
		this->pitch = newPitch;
	}
	else {
		this->pitch += newPitch;
	}
}

float CameraHandler::GetYaw()
{
	return this->yaw;
}

void CameraHandler::SetYaw(float newYaw, bool add)
{
	if (!add) {
		this->yaw = newYaw;
	}
	else {
		this->yaw += newYaw;
	}
}

void CameraHandler::UpdateCamera()
{
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->pitch, this->yaw, this->roll);

	DirectX::XMVECTOR camPos = DirectX::XMVector3TransformCoord(this->cameraPos, camRotationMatrix);
	DirectX::XMVECTOR lookAt = DirectX::XMVector3TransformCoord(this->lookAt, camRotationMatrix);
	DirectX::XMVECTOR camUp = DirectX::XMVector3TransformCoord(this->cameraUp, camRotationMatrix);

	this->viewMatrix = DirectX::XMMatrixLookAtLH(camPos, lookAt, camUp);
}
