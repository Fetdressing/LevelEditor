#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

class Material{
public:
	char *name;
	MaterialData materialData;

	Material(){}
	~Material(){
		free(name);
	}
	//skapa constantbuffer h�r???
	void EmptyVariables(){
		free(name);
	}

};