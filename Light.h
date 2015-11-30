#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#endif

#include "ObjectData.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara k�nna till att det finns en class som heter Transform, denna pekar p� transform och transform pekar p� denna

class Light{
	struct LightCBufferData {
		LightData lData;
		TransformData tData;
	};
public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	char *name;
	LightData lightData;

	Transform *transform = nullptr; //s� jag alltid kan komma �t transformv�rdet direkt
	LightCBufferData lightCBufferData; //datan som skickas in i cbuffern
	ID3D11Buffer *lightCbuffer = nullptr; //h�r ligger den storade cameradatan

	Light(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		
	}
	~Light(){
		delete(name);
		lightCbuffer->Release();
	}

	void UpdateCBuffer();
	void CreateCBuffer();

	void EmptyVariables() {
		delete(name);
		//cameraCbuffer->Release(); //inte s�kert jag vill detta, kanske remapa ist�llet! updatesubresource
	}
};