#pragma once

#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#endif

#include "Mesh.h"
#include "Light.h"
#include "Transform.h"
#include "Material.h"
#include "ObjectData.h"
//behöver tillgång till dessa, vill vi flytta fileloadern så måste dessa oxå finnas där

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class FileHandler {
public:
	FileHandler();
	~FileHandler();

	ofstream ofs; //skriv till fil
	ifstream ifs; //läs från fil
	int max_Name_Size = 100;

	void SaveScene(int maxNameSize, char* sceneName,
		vector<Material*> &materials,
		vector<Transform*> &allTransforms,
		vector<Transform*> &allMeshTransforms,
		vector<Transform*> &allLightTransforms);


	void LoadScene(); //scene class??

private:
	int CorrectName(char *&referenceName); //tänk på att de sparade namnen kommer ha färre tecken än de som laddas in från maya

	void SaveMainHeader(int sceneNameSize, char* sceneName, int nrMats, int nrTransforms, int nrMeshes, int nrLights);

	void SaveMaterials(int nrMats, vector<Material*> &materials);
	void SaveTransforms(int nrTransforms, vector<Transform*> &allTransforms);
	void SaveMeshes(int nrMeshes, vector<Transform*> &allMeshTransforms);
	void SaveLights(int nrLights, vector<Transform*> &allLightTransforms);
	//Scene currentScene;
	
};