#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

class Material{
public:
	ID3D11Device * gDevice = nullptr;
	ID3D11DeviceContext * gDeviceContext = nullptr;

	char *name;
	MaterialData materialData;
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
		delete(name);
		materialCbuffer->Release();

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
	void UpdateCBuffer(){
		//updatesubresource med den nya materialData
		gDeviceContext->UpdateSubresource(materialCbuffer, 0, NULL, &materialData, 0, 0);
	}
	void CreateCBuffer(){
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(MaterialData); //kolla så den är 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &materialData; //ger den startvärde, default, använd updatesubresource sen
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &materialCbuffer);
	}

	void EmptyVariables(){
		delete(name);
		//materialCbuffer->Release(); //inte säkert jag vill detta, kanske remapa istället! updatesubresource??
	}

};