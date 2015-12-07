#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#endif

#include "ObjectData.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara k�nna till att det finns en class som heter Transform, denna pekar p� transform och transform pekar p� denna

class CameraObj{
	struct CameraCBufferData {
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	char *name;
	CameraData cameraData;

	Transform *transform = nullptr; //s� jag alltid kan komma �t transformv�rdet direkt
	CameraCBufferData cameraCBufferData;
	ID3D11Buffer *cameraCbuffer = nullptr; //h�r ligger den storade cameradatan

	XMMATRIX view;
	XMMATRIX projection;

	CameraObj(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		//CreateCameraCBuffer();
	}
	~CameraObj(){
		delete(name);
		cameraCbuffer->Release();
	}

	void UpdateCBuffer(UINT screenWidth, UINT screenHeight);
	void CreateCBuffer();

	void EmptyVariables() {
		delete(name);
		//cameraCbuffer->Release(); //inte s�kert jag vill detta, kanske remapa ist�llet! updatesubresource
	}
};