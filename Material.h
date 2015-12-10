#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

#include "ObjectData.h"

class Material{
public:
	struct MaterialCBufferData {
		float diffuse;
		float color[3];
		float ambColor[3];
		float specColor[3];
		float specCosine;
		float specEccentricity;
		float specRollOff;
		float padding[3];

		MaterialCBufferData()
		{
			padding[0] = padding[1] = padding[2] = 0;
		}
	};
	
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	void *materialDataP = nullptr; //pointer to the current values, för att ta bort messaget som varit mallocat
	char *name;
	MaterialData materialData;
	MaterialCBufferData materialCBufferData;
	ID3D11Buffer *materialCbuffer = nullptr; //här ligger den storade materialdatan

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
		
	}
	~Material(){
		//delete(name); den är statiskt allokerad
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
	//skapa constantbuffer här???
	void UpdateCBuffer();
	void CreateCBuffer();

	void EmptyVariables(){
		free(materialDataP);
		//materialCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource??
	}

};