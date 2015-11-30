#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#endif

#include "ObjectData.h"
#include "Transform.h"

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

	void UpdateCBuffer() {
		//updatesubresource med den nya materialData
		cameraCBufferData.cData = cameraData;
		cameraCBufferData.tData = transform->transformData;
		gDeviceContext->UpdateSubresource(cameraCbuffer, 0, NULL, &cameraCBufferData, 0, 0);
	}
	void CreateCBuffer() {
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(CameraCBufferData); //kolla så den är 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &cameraCBufferData; //ger den startvärde, default, använd updatesubresource sen
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &cameraCbuffer);
	}

	void EmptyVariables() {
		delete(name);
		//cameraCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource
	}
};