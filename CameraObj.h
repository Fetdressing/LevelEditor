#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#endif

#include "ObjectData.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara känna till att det finns en class som heter Transform, denna pekar på transform och transform pekar på denna

class CameraObj{
	struct CameraCBufferData {
		CameraData cData;
		TransformData tData;
	};

public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	char *name;
	CameraData cameraData;

	Transform *transform = nullptr; //så jag alltid kan komma åt transformvärdet direkt
	CameraCBufferData cameraCBufferData;
	ID3D11Buffer *cameraCbuffer = nullptr; //här ligger den storade cameradatan


	CameraObj(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		//CreateCameraCBuffer();
	}
	~CameraObj(){
		delete(name);
		cameraCbuffer->Release();
	}

	void UpdateCBuffer();
	void CreateCBuffer();

	void EmptyVariables() {
		delete(name);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};