#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#endif

class Material{
public:
	ID3D11Device * gDevice = nullptr;
	char *name;
	MaterialData materialData;
	ID3D11Buffer *materialCbuffer = nullptr; //h�r ligger den storade materialdatan

	Material(ID3D11Device *gDevice){
		this->gDevice = gDevice;

	}
	~Material(){
		free(name);
	}
	//skapa constantbuffer h�r???
	void CreateMaterialCBuffer(){
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(MaterialData); //kolla s� den �r 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &materialData; //r�tt??
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &materialCbuffer);
	}

	void EmptyVariables(){
		delete(name);
		materialCbuffer->Release(); //inte s�kert jag vill detta, kanske remapa ist�llet!
	}

};