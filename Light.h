#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#endif

#include "ObjectData.h"

class Light{
public:
	char *name;
	LightData lightData;

	Light(){}
	~Light(){
		free(name);
	}
};