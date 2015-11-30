#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
#endif


#include "ObjectData.h"
#include "Mesh.h"
#include "CameraObj.h"
#include "Light.h"

class Transform{
	struct TransformCBufferData{
		XMFLOAT4 world;
	};
public:
	//char* name;
	//Transform transformData;
	char *name;
	char *parentName;
	TransformData transformData;

	Transform *parent = nullptr; //anv�nd parenten och h�mta dess transformation
	TransformCBufferData transformCBufferData;
	ID3D11Buffer *transformCBuffer;

	//kommer ha en av dessa, placera dem i olika vektorer!
	Mesh *mesh = nullptr;
	Light *light = nullptr;
	CameraObj *camera = nullptr; //fick d�pa om den till obj pga oskeeaaar

	Transform(ID3D11Device* gd, ID3D11DeviceContext* gdc){
		this->gDevice = gd; //freea dessa inte h�r, g�rs i main duuh
		this->gDeviceContext = gdc;
		CreateTransformCBuffer();
	}
	Transform(){}
	~Transform(){
		delete(name);
	}
	void UpdateCBuffer(){
		//updatesubresource med den nya transformData
		//transformdata ligger p� plats 0, material p� 1, osv
		gDeviceContext->UpdateSubresource(transformCBuffer, 0, NULL, &transformCBufferData, 0, 0); //skapa en separat struct f�r transformdata som ska in i shadern, world osv
	}
	void CreateTransformCBuffer(){ //gl�m inte parentens skit?
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(MaterialData); //kolla s� den �r 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &transformData; //ger den startv�rde, default, anv�nd updatesubresource sen
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &transformCBuffer);
	}

	void EmptyVariables(){
		delete(name);
		delete(parentName);
		transformCBuffer->Release();
	}
private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
};