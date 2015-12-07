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
		XMFLOAT4X4 world;
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

	Transform(ID3D11Device* gd, ID3D11DeviceContext* gdc)
	{
		this->gDevice = gd; //freea dessa inte h�r, g�rs i main duuh
		this->gDeviceContext = gdc;
		CreateTransformCBuffer();
	}
	Transform()
	{}
	~Transform()
	{ //kanske deleta saker som mesh, men m�jligt vi skickar separata deletes f�r dem, detsamma g�ller children
		delete(name);
	}
	void UpdateCBuffer();
	void CreateTransformCBuffer();

	void EmptyVariables(){
		delete(name);
		delete(parentName);
		transformCBuffer->Release();
	}
private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
};