#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
#endif


#include "ObjectData.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

class Transform{
public:
	//char* name;
	//Transform transformData;	
	char *name;
	char *parentName;
	Transform *parent; //anv�nd parenten och h�mta dess transformation
	bool hasParent;
	TransformData transformData;
	
	//kommer ha en av dessa, placera dem i olika vektorer!
	Mesh *mesh = nullptr;
	Light *light = nullptr;
	CameraObj *camera = nullptr; //fick d�pa om den till obj pga oskeeaaar

	Transform(ID3D11Device* gd, ID3D11DeviceContext* gdc){
		this->gDevice = gd; //freea dessa inte h�r, g�rs i main duuh
		this->gDeviceContext = gdc;
	}
	Transform(){}
	~Transform(){
		delete(name);
	}
	void EmptyVariables(){
		delete(name);
		delete(parentName);
	}
private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;
};