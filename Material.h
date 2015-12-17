#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

#include "Entity.h"

class Material : public Entity{
public:
	struct MaterialCBufferData {
		float diffuse;
		float color[3];
		float ambColor[3];
		float specColor[3];
		float specCosine;
		float specEccentricity;
		float specRollOff;
		UINT hasTexture;
		float padding[2];

		MaterialCBufferData()
		{
			diffuse = 0;
			color[0] = color[1] = color[2] = 0;
			ambColor[0] = ambColor[1] = ambColor[2] = 0;
			specColor[0] = specColor[1] = specColor[2] = 0;
			specCosine = 0;
			specEccentricity = 0;
			specRollOff = 0;
			hasTexture = 0;
			padding[0] = padding[1] = 0;
		}
	};
	
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	void *materialDataP = nullptr; //pointer to the current values, f�r att ta bort messaget som varit mallocat
	char *name;
	char *textureName; //store filpathsen, �ven om jag har texturen s� kommer vi beh�va pathsen f�r fileformatet sen!
	char *normalMapName;
	char *specularMapName;
	char *emissionMapName;
	char dummyName[100]; //dummy variable som anv�nds som default namn, beh�ver inte deallokeras
	MaterialData materialData;
	MaterialCBufferData materialCBufferData;
	ID3D11Buffer *materialCbuffer = nullptr; //h�r ligger den storade materialdatan

	//char *textureName;
	ID3D11Resource *diffuseTexture;
	ID3D11ShaderResourceView *diffuseTextureView;

	ID3D11Resource *normalTexture;
	ID3D11ShaderResourceView *normalTextureView;

	ID3D11Resource *specularTexture;
	ID3D11ShaderResourceView *specularTextureView;

	ID3D11Resource *emissionTexture;
	ID3D11ShaderResourceView *emissionTextureView;

	Material(ID3D11Device *gDevice, ID3D11DeviceContext *gDevC){
		this->gDevice = gDevice;
		this->gDeviceContext = gDevC;

		name = dummyName; //s�tter den till dummy namnet bara f�r att ha ett default
		textureName = dummyName;
		normalMapName = dummyName;
		specularMapName = dummyName;
		emissionMapName = dummyName;
	}
	~Material(){
		//delete(name); den �r statiskt allokerad
		materialCbuffer->Release();
		free(materialDataP);

		diffuseTexture->Release();
		diffuseTextureView->Release();
		normalTexture->Release();
		normalTextureView->Release();
		specularTexture->Release();
		specularTextureView->Release();
		emissionTexture->Release();
		emissionTextureView->Release();
	}
	//skapa constantbuffer h�r???
	void UpdateCBuffer();
	void CreateCBuffer();
	void CreateTexture(char* filePath, ID3D11Resource *&texture, ID3D11ShaderResourceView *&textureView);

	void EmptyVariables(){
		free(materialDataP);
		//materialCbuffer->Release(); //inte s�kert jag vill detta, kanske remapa ist�llet! updatesubresource??
	}

};