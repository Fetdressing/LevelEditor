#pragma once
#ifndef MAYALOADER_H
#define MAYALOADER_H
#endif


#include <vector>
#include <Windows.h>
#include <ostream>
#include <iostream>
#include <string>

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"
#include "Mutex.h"

using namespace std;

class MayaLoader{
	
public:	
	MayaLoader(ID3D11Device* gd, ID3D11DeviceContext* gdc);
	MayaLoader();
	~MayaLoader();
	void DrawScene();

	//void StartSyncing(); //startar hela processen
	void CreateFileMaps(unsigned int messageFilemapSize);

	void TryReadAMessage();

	void ReadTransform(int i); //parameter f�r edited/added/removed
	void ReadMesh(int i);
	void ReadMeshData(size_t offSetStart, size_t reducedMessageSize); //bara s� det inte ska bli lika mkt kod
	void ReadLight(int i);
	void ReadMaterial(int i);
	void ReadCamera(int i);
	
	//f�r tester
	void TryWriteAMessage();

private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;

	//data om filemapen
	struct FilemapInfo{
		size_t head_ByteOffset; //offset in bytes from beginning of the shared memory
		size_t tail_ByteOffset; //offset in bytes from beginning of the shared memory
		size_t non_accessmemoryOffset; //memory in beginning of file thats no touchy, so that head and tail won't get to each other
		//size_t totalConsumers;
		size_t messageFilemap_Size;

		FilemapInfo(){
			head_ByteOffset = 0;
			tail_ByteOffset = 0;
			non_accessmemoryOffset = 10;
			//totalConsumers = 0;
			messageFilemap_Size = 0; //storleken p� filemapen med meddelanden
		}

	};
	FilemapInfo fileMapInfo;

	void SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize); //s�tt negativa v�rden om den inte ska �ndras!
	HANDLE hMessageFileMap;
	LPVOID mMessageData;
	unsigned int mSize = 1 << 13; //2^15, denna s�tts vid skapelsen av filemapen, parameter v�rdet till funktionen
	//will hold information about where heads and tails are for example, aswell as where the free memory is in the other shared memory
	HANDLE hInfoFileMap;
	LPVOID mInfoData;
	unsigned int mInfoSize = 1 << 6;

	size_t thisApplication_filemap_MemoryOffset = 0; //hur l�ngt ifr�n starten i bytes den �r p� filemapen med meddelanden
	int delay = 10; //hur l�nge den ska sleepa mellan varje l�snings f�rs�k

	struct MessageHeader{
		MessageHeader(){
			nodeType = 0;
			messageType = 0;
			//consumersLeft = 0; //consumers kvar att passera!
			byteSize = 0;
			bytePadding = 0;
			nameElementSize = 0;
		}

		int nodeType;
		int messageType;
		//int consumersLeft;
		size_t byteSize;
		size_t bytePadding;
		int nameElementSize; //hur stort �r namnet i karakt�rer
		char objectName[100]; //dynamisk eller inte, char compare funktionen vad g�ra?
	};
	MessageHeader messageHeader;
	bool headerDidFit; //true om den fick plats p� denna sidan av filemapen, annars �r den p� andra, anv�nds vid l�sning av meddelanden
	
	//har kvar vissa av dessa structsen �ven om de �r lite on�diga d� alla v�rden i vissa redan ligger i en full struct, men vill h�lla det cleant med namngivning :-)
	struct TransformMessage{
		char parentName[100];
		TransformData transformData;
	};
	struct CameraMessage{
		char transformName[100];
		CameraData cameraData;
	};
	struct MeshMessage{
		char transformName[100];
		MeshData *meshData;
	};
	struct MaterialMessage{ //namnet p� den ligger i headern sen
		MaterialData materialData;
	};
	struct LightMessage{
		//ljusv�rden
		char transformName[100];
		LightData lightdata;
	};

	TransformMessage *transformMessage;
	CameraMessage *cameraMessage;
	MeshMessage *meshMessage;
	MaterialMessage *materialMessage;
	LightMessage *lightMessage;

	size_t transformMessage_MaxSize = 512;
	size_t cameraMessage_MaxSize = 512;
	size_t meshMessage_MaxSize = 4096; //kan ju fan inte h�rdkodas!
	size_t materialMessage_MaxSize = 512;
	size_t lightMessage_MaxSize = 512;

	//vector<GameObject*> gameObjects;
	vector<Material*> materials;
	vector<Transform*> allTransforms;
	vector<Transform*> allMeshTransforms;
	vector<Transform*> allLightTransforms;
	vector<Transform*> allCameraTransforms;

	void MeshChange(MessageHeader mh, MeshMessage *mm); //l�gger till ett nytt objekt om det inte redan finns eller updaterar en gammal, tar hand om den aktualla meshen
	void MeshAdded(MessageHeader mh, MeshMessage *mm);

	void TransformChange(MessageHeader mh, TransformMessage *mm);
	void TransformAdded(MessageHeader mh, TransformMessage *mm);
	void TransformDeleted(MessageHeader mh);

	void MaterialAdded(MessageHeader mh, MaterialMessage *mm);
	void MaterialChange(MessageHeader mh, MaterialMessage *mm);
	void MaterialDeleted(MessageHeader mh);

	void LightAdded(MessageHeader mh, LightMessage *mm);
	void LightChange(MessageHeader mh, LightMessage *mm);

	void CameraAdded(MessageHeader mh, CameraMessage *mm);
	void CameraChange(MessageHeader mh, CameraMessage *mm);
};