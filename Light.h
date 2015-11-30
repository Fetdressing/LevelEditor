#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#endif

#include "ObjectData.h"

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

	Transform *transform = nullptr; //så jag alltid kan komma åt transformvärdet direkt
	LightCBufferData lightCBufferData; //datan som skickas in i cbuffern
	ID3D11Buffer *lightCbuffer = nullptr; //här ligger den storade cameradatan

	Light(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC) {
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;
		
	}
	~Light(){
		delete(name);
		lightCbuffer->Release();
	}

	void UpdateCBuffer() {
		//updatesubresource med den nya materialData
		lightCBufferData.lData = lightData;
		lightCBufferData.tData = transform->transformData;
		gDeviceContext->UpdateSubresource(lightCbuffer, 0, NULL, &lightCBufferData, 0, 0);
	}
	void CreateCBuffer() {
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(LightCBufferData); //kolla så den är 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &lightCBufferData; //ger den startvärde, default, använd updatesubresource sen
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &lightCbuffer);
	}

	void EmptyVariables() {
		delete(name);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};