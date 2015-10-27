#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#endif

#include "ObjectData.h"

class CameraObj{
public:
	char *name;
	CameraData cameraData;

	CameraObj(){

	}
	~CameraObj(){
		free(name);
	}
};