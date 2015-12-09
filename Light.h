#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#endif

#include "ObjectData.h"
//#include "Transform.h"

class Transform; //forward declaration, den ska bara känna till att det finns en class som heter Transform, denna pekar på transform och transform pekar på denna

class Light{
	struct LightCBufferData {
		LightData lData;
		TransformData tData; //behöver vara här coz vi vill ha allt ljusstuff i en constant buffer, så hämta värdet från transformen
	};
public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	void *lightDataP = nullptr;
	char *name;
	LightData lightData;

	Transform *transform = nullptr; //så jag alltid kan komma åt transformvärdet direkt
	LightCBufferData lightCBufferData; //datan som skickas in i cbuffern
	ID3D11Buffer *lightCbuffer = nullptr; //här ligger den storade cameradatan

	Light(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		
	}
	~Light(){
		free(lightDataP);
		lightCbuffer->Release();
	}

	void UpdateCBuffer();
	void CreateCBuffer();

	void EmptyVariables() {
		free(lightDataP);
		//delete[] (name);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};