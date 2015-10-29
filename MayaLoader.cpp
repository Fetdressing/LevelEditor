#include "MayaLoader.h"

Mutex mutexInfo("__info_Mutex__");
MayaLoader::MayaLoader(ID3D11Device* gd, ID3D11DeviceContext* gdc){
	this->gDevice = gd;
	this->gDeviceContext = gdc;
	CreateFileMaps(4096);

	transformMessage = (TransformMessage*)malloc(transformMessage_MaxSize);
	meshMessage = (MeshMessage*)malloc(meshMessage_MaxSize);
	cameraMessage = (CameraMessage*)malloc(cameraMessage_MaxSize);
	lightMessage = (LightMessage*)malloc(lightMessage_MaxSize);
	materialMessage = (MaterialMessage*)malloc(materialMessage_MaxSize);

	/*TryWriteAMessage();
	TryReadAMessage();*/
}
MayaLoader::~MayaLoader(){
	

	UnmapViewOfFile((LPCVOID)mMessageData);
	CloseHandle(hMessageFileMap);

	UnmapViewOfFile((LPCVOID)mInfoData);
	CloseHandle(hInfoFileMap);
	
	free(transformMessage);
	free(meshMessage);
	free(cameraMessage);
	free(lightMessage);
	free(materialMessage);

}

void MayaLoader::CreateFileMaps(unsigned int messageFilemapSize){
	//messagefilemap
	mSize = messageFilemapSize;
	hMessageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR) "messageFileMap");

	mMessageData = MapViewOfFile(hMessageFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (hMessageFileMap == NULL){
		cout << "Couldn't create filemap\n";
	}

	if (GetLastError() == ERROR_ALREADY_EXISTS){
		cout << "Filemap exists, you get an handle to it!\n";
		
	}
	else{
		cout << "Creating new filemap\n";		
	}
	

	//info filemapen
	hInfoFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mInfoSize,
		(LPCWSTR) "infoFileMap");

	mInfoData = MapViewOfFile(hInfoFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	SetFilemapInfoValues(0, 0, 0, mSize); //storar de i filemapen oxå! sätt negativa värden om man inte vill nått värde ska ändras :)
	
	if (hInfoFileMap == NULL){
		cout << "Couldn't create infofilemap\n";
	}
	//FilemapInfo fmInfo;
	if (GetLastError() == ERROR_ALREADY_EXISTS){
		cout << "Infofilemap exists, you get an handle to it!\n";
	}
	else{ //first, sätter de första värdena på filemapinfon
		cout << "Creating new infofilemap\n";


	}
}

void MayaLoader::SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize){
	mutexInfo.Lock();	
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	if (headPlacement >= 0)
		fileMapInfo.head_ByteOffset = headPlacement;
	if (tailPlacement >= 0)
		fileMapInfo.tail_ByteOffset = tailPlacement;
	if (nonAccessMemoryPlacement >= 0)
		fileMapInfo.non_accessmemoryOffset = nonAccessMemoryPlacement;
	if (messageFileMapTotalSize > 0)
		fileMapInfo.messageFilemap_Size = messageFileMapTotalSize;
	
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();
}

void MayaLoader::DrawScene(){
	UINT32 vertexSize2 = sizeof(float) * 8;
	UINT32 offset2 = 0;
	//set rätt constantbuffers, ljus, kamera och material stuff!
	for (int i = 0; i < allMeshTransforms.size(); i++){
		gDeviceContext->IASetVertexBuffers(0, 1, &allMeshTransforms[i]->mesh->vertexBuffer, &vertexSize2, &offset2);
		gDeviceContext->Draw(allMeshTransforms[i]->mesh->nrVertices, 0);
	}
}

void MayaLoader::TryReadAMessage(){
	//bool canMoveOne = false; //kan jag flyttade vidare denna consumer i minnet eller måste jag vänta på producern?
	////size_t messageSize;
	//while (canMoveOne == false){ //kolla data från infofilemappen
		//mutexInfo.Lock();
		memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo)); //hämta filemapinfo datan om jag har fått mutexInfo till den
		//mutexInfo.Unlock();
		//messageSize = messageFile.header.byteSize + messageFile.header.bytePadding;
		if (thisApplication_filemap_MemoryOffset != fileMapInfo.head_ByteOffset){ //sålänge consumern inte har hunnit till headern
			//KAN LÄSA!!
			if ((thisApplication_filemap_MemoryOffset + sizeof(MessageHeader)) < mSize){ //headern ligger på denna sidan!
				headerDidFit = true;
				memcpy(&messageHeader, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset, sizeof(MessageHeader)); //läs headern som vanligt
			}
			else{ //headern ligger på andra sidan, headern får inte plats
				headerDidFit = false;
				memcpy(&messageHeader, (unsigned char*)mMessageData, sizeof(MessageHeader)); //läs headern i början på filen
				thisApplication_filemap_MemoryOffset = 0; //flytta över hela meddelandet till andra sidan
			}
			//enum NodeTypes { TTransform, TMesh, TCamera, TLight, TMaterial };
			
			switch (messageHeader.nodeType){
				case 0:
					ReadTransform(messageHeader.messageType);					
					break;
				case 1:
					ReadMesh(messageHeader.messageType);
					break;
				case 2:
					//nodeType = TCamera;
					break;
				case 3:
					//nodeType = TLight;
					break;
				case 4:
					//nodeType = TMaterial;
					break;
				default:
					printf("Invalid message ID");
			}

		}

}

void MayaLoader::ReadTransform(int i){
	//LÄS MEDDELANDE OCH HEADER
	//får bara headern plats eller får oxå meddelandet plats?
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(transformMessage, (unsigned char*)mMessageData, messageHeader.byteSize - sizeof(MessageHeader)); //läser i början på filen utan nån offset
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding - sizeof(MessageHeader);
		}
		else{ //meddelandet får plats!!
			memcpy(transformMessage, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + thisApplication_filemap_MemoryOffset + messageHeader.bytePadding;
		}
		
	}
	else{ //headern ligger på andra sidan, headern får inte plats
		//messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			memcpy(transformMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding;
		}
		
	}

	if (i == 1){
		TransformAdded(messageHeader, transformMessage);
	}
	else if (i == 2){
		TransformChange(messageHeader, transformMessage); //tar hand om den aktualla meshen
	}
	else if (i == 3){
		TransformDeleted(messageHeader);
	}


	//flytta tailen
	mutexInfo.Lock();
	cout << "Move tail!!!!!" << thisApplication_filemap_MemoryOffset << "\n";
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = thisApplication_filemap_MemoryOffset;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();


}
void MayaLoader::ReadMaterial(int i){
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(materialMessage, (unsigned char*)mMessageData, messageHeader.byteSize - sizeof(MessageHeader)); //läser i början på filen utan nån offset
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding - sizeof(MessageHeader);
		}
		else{ //meddelandet får plats!!
			memcpy(materialMessage, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + thisApplication_filemap_MemoryOffset + messageHeader.bytePadding;
		}

	}
	else{ //headern ligger på andra sidan, headern får inte plats
		//messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			memcpy(materialMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding;
		}

	}

	if (i == 1){
		MaterialAdded(messageHeader, materialMessage);
	}
	else if (i == 2){
		MaterialChange(messageHeader, materialMessage); //tar hand om den aktualla meshen
	}
	else if (i == 3){
		MaterialDeleted(messageHeader);
	}


	//flytta tailen
	mutexInfo.Lock();
	cout << "Move tail!!!!!" << thisApplication_filemap_MemoryOffset << "\n";
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = thisApplication_filemap_MemoryOffset;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadMesh(int i){
	//LÄS MEDDELANDE OCH HEADER
	//får bara headern plats eller får oxå meddelandet plats?
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			ReadMeshData(0, sizeof(MessageHeader));
			//memcpy(meshMessage, (unsigned char*)mMessageData, messageHeader.byteSize - sizeof(MessageHeader)); //läser i början på filen utan nån offset
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding - sizeof(MessageHeader);
		}
		else{ //meddelandet får plats!!
			ReadMeshData(thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), sizeof(MessageHeader));
			//memcpy(meshMessage, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + thisApplication_filemap_MemoryOffset + messageHeader.bytePadding;
		}

	}
	else{ //headern ligger på andra sidan, headern får inte plats
		//messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			ReadMeshData(sizeof(MessageHeader), sizeof(MessageHeader));
			//memcpy(meshMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding;
		}

	}
	if (i == 1){
		MeshAdded(messageHeader, meshMessage);
	}
	else if (i == 2){
		MeshChange(messageHeader, meshMessage); //tar hand om den aktualla meshen
	}
	//flytta tailen
	mutexInfo.Lock();
	cout << "Move tail!!!!!" << thisApplication_filemap_MemoryOffset << "\n";
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = thisApplication_filemap_MemoryOffset;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadMeshData(size_t offSetStart, size_t reducedMessageSize){
	//memcpy(meshMessage, (unsigned char*)mMessageData + offSetStart, sizeof(int));
	//int nrVert = messageHeader.nrVerts;
	//int nrIndecies = messageHeader.nrIndecies;
	//
	memcpy(meshMessage->objectName, (unsigned char*)mMessageData + offSetStart, sizeof(meshMessage->objectName));
	UINT offset = (sizeof(char) * 100);
	memcpy(meshMessage->transformName, (unsigned char*)mMessageData + offSetStart + offset, sizeof(meshMessage->transformName));
	offset += (sizeof(char) * 100);
	memcpy(meshMessage->meshData, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * 5);

	offset += sizeof(int) * 5; //7

	memcpy(meshMessage->meshData->positions, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float3) * meshMessage->meshData->nrPos);
	offset += sizeof(Float3) * meshMessage->meshData->nrPos;
	memcpy(meshMessage->meshData->normals, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float3) * meshMessage->meshData->nrNor);
	offset += sizeof(Float3) * meshMessage->meshData->nrNor;
	memcpy(meshMessage->meshData->uvs, (unsigned char*)mMessageData + offSetStart + offset, sizeof(Float2) * meshMessage->meshData->nrUV);
	offset += sizeof(Float2) * meshMessage->meshData->nrUV;

	
	memcpy(meshMessage->meshData->indexPositions, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
	offset += sizeof(int) * meshMessage->meshData->nrI;
	memcpy(meshMessage->meshData->indexNormals, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
	offset += sizeof(int) * meshMessage->meshData->nrI;
	memcpy(meshMessage->meshData->indexUVs, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->nrI);
	offset += sizeof(int) * meshMessage->meshData->nrI;

	memcpy(meshMessage->meshData->trianglesPerFace, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * meshMessage->meshData->triangleCount);

	//meshMessage->meshData.vertices = new Vertex[nrVert];
	//meshMessage->meshData.indecies = new Index[nrIndecies];
	//memcpy(meshMessage->meshData.vertices, (unsigned char*)mMessageData + offSetStart, messageHeader.byteSize - reducedMessageSize - (sizeof(Index) * nrIndecies));
	//memcpy(meshMessage->meshData.indecies, (unsigned char*)mMessageData + offSetStart + (sizeof(Vertex) * nrVert), messageHeader.byteSize - reducedMessageSize - (sizeof(Vertex) * nrVert));

}
void MayaLoader::ReadLight(int i){
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(lightMessage, (unsigned char*)mMessageData, messageHeader.byteSize - sizeof(MessageHeader)); //läser i början på filen utan nån offset
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding - sizeof(MessageHeader);
		}
		else{ //meddelandet får plats!!
			memcpy(lightMessage, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + thisApplication_filemap_MemoryOffset + messageHeader.bytePadding;
		}

	}
	else{ //headern ligger på andra sidan, headern får inte plats
		//messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			memcpy(lightMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding;
		}

	}

	if (i == 1){
		LightAdded(messageHeader, lightMessage);
	}
	else if (i == 2){
		LightChange(messageHeader, lightMessage); //tar hand om den aktualla meshen
	}
	
	//flytta tailen
	mutexInfo.Lock();
	cout << "Move tail!!!!!" << thisApplication_filemap_MemoryOffset << "\n";
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = thisApplication_filemap_MemoryOffset;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadCamera(int i){
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(cameraMessage, (unsigned char*)mMessageData, messageHeader.byteSize - sizeof(MessageHeader)); //läser i början på filen utan nån offset
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding - sizeof(MessageHeader);
		}
		else{ //meddelandet får plats!!
			memcpy(cameraMessage, (unsigned char*)mMessageData + thisApplication_filemap_MemoryOffset + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + thisApplication_filemap_MemoryOffset + messageHeader.bytePadding;
		}

	}
	else{ //headern ligger på andra sidan, headern får inte plats
		//messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((thisApplication_filemap_MemoryOffset + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			memcpy(cameraMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize - sizeof(MessageHeader));
			thisApplication_filemap_MemoryOffset = messageHeader.byteSize + messageHeader.bytePadding;
		}

	}

	if (i == 1){
		CameraAdded(messageHeader, cameraMessage);
	}
	else if (i == 2){
		CameraChange(messageHeader, cameraMessage); //tar hand om den aktualla meshen
	}

	//flytta tailen
	mutexInfo.Lock();
	cout << "Move tail!!!!!" << thisApplication_filemap_MemoryOffset << "\n";
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = thisApplication_filemap_MemoryOffset;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}

void MayaLoader::TryWriteAMessage(){
	MessageHeader messageFileH;
	messageFileH.nodeType = 0;
	messageFileH.byteSize = 1024;
	messageFileH.bytePadding = 0;
	//messageFileH.objectName = "hej";
	
	//TransformMessage *t;
	transformMessage->transformData.pos = Float3(1, 1, 1);
	transformMessage->transformData.rot = Float3(2, 1, 7);
	transformMessage->transformData.scale = Float3(1, 0, 2);
	memcpy((unsigned char*)mMessageData + fileMapInfo.head_ByteOffset, &messageFileH, sizeof(MessageHeader));
	memcpy((unsigned char*)mMessageData + fileMapInfo.head_ByteOffset + sizeof(MessageHeader), transformMessage, messageFileH.byteSize - sizeof(MessageHeader));

	//flytta headern (i filemapen, det lokala värdet ändras lite tidigare)
	mutexInfo.Lock(); //behöver inte låsa förrens här becuz producern är den ända som faktiskt kan ändra headern, fast samtidigt kan den ändra andra värden???
	memcpy(&fileMapInfo, mInfoData, sizeof(FilemapInfo)); //hämta filemapinfo datan om jag har fått mutexInfo till den
	fileMapInfo.head_ByteOffset = 0;
	memcpy(mInfoData, &fileMapInfo, sizeof(FilemapInfo)); //skriv till infofilemappen
	mutexInfo.Unlock();
}


void MayaLoader::MeshAdded(MessageHeader mh, MeshMessage *mm){
	char* meshName = mh.objectName;
	char* transformName = mm->transformName;
	Transform *meshTransform = nullptr; //hitta den
	Mesh *activeMesh = nullptr;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			meshTransform = allTransforms[i];
			break;
		}
	}
	if (meshTransform == nullptr){
		printf("Hitta inte transformen");
	}

	activeMesh = meshTransform->mesh;
	
	activeMesh = new Mesh(gDevice, gDeviceContext);

	activeMesh->name = mh.objectName;
	activeMesh->meshData = mm->meshData;
	/*activeMesh->meshData.nrVerts = mh.nrVerts;
	activeMesh->meshData.nrIndecies = mh.nrIndecies;

	activeMesh->meshData.vertices = mm->meshData.vertices;
	activeMesh->meshData.indecies = mm->meshData.indecies;*/

	activeMesh->CreateBuffers();
	allMeshTransforms.push_back(meshTransform); //skickar in denna transform i allMeshTransforms oxå!! så den kommer vara refererad i båda vektorerna
	
}
void MayaLoader::MeshChange(MessageHeader mh, MeshMessage *mm){ //MÅSTE HA TRANSFORMEN FÖRST, SEN SKAPA ETT MESH OBJEKT I TRANSFORMEN
	char* meshName = mh.objectName;
	char* transformName = mm->transformName;
	Transform *meshTransform = nullptr; //hitta den
	Mesh *activeMesh = nullptr;

	for (int i = 0; i < allMeshTransforms.size(); i++){
		if (strcmp(transformName, allMeshTransforms[i]->name) == 0){
			meshTransform = allMeshTransforms[i];
			break;
		}
	}
	if (meshTransform == nullptr){
		printf("Hitta inte transformen");
	}

	activeMesh = meshTransform->mesh;
	activeMesh->EmptyBuffersAndArrays(); //viktigt att göra dessa, annars kommer variablerna gå lost utan referens!!
	activeMesh->EmptyVariables();

	activeMesh->name = mh.objectName;
	activeMesh->meshData = mm->meshData;
	//activeMesh->meshData.nrVerts = mh.nrVerts;
	//activeMesh->meshData.nrIndecies = mh.nrIndecies;

	//activeMesh->meshData.vertices = mm->meshData.vertices;
	//activeMesh->meshData.indecies = mm->meshData.indecies;

	activeMesh->RemapVertexBuffer();	

}

void MayaLoader::TransformAdded(MessageHeader mh, TransformMessage *mm){
	char* transformName = mh.objectName;
	Transform *transform = new Transform(gDevice, gDeviceContext); //hitta den

	transform->name = transformName;
	transform->transformData = mm->transformData;
	transform->parentName = mm->parentName;
	
	if (transform->parentName[0] == '0'){
		transform->hasParent = false;
	}
	else{
		transform->hasParent = true;
	}

	allTransforms.push_back(transform);
}
void MayaLoader::TransformChange(MessageHeader mh, TransformMessage *mm){
	char* transformName = mh.objectName;
	Transform *transform = nullptr;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			transform = allTransforms[i];
			break;
		}
	}

	transform->EmptyVariables();

	transform->name = transformName;
	transform->parentName = mm->parentName;
	transform->transformData = mm->transformData;

	if (transform->parentName[0] == '0'){
		transform->hasParent = false;
	}
	else{
		transform->hasParent = true;
	}

}
void MayaLoader::TransformDeleted(MessageHeader mh){ //ta bort från alla vektorer!!! inte bara allTransforms!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	char* objName = messageHeader.objectName;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(objName, allTransforms[i]->name) == 0){
			delete allTransforms[i];
			allTransforms.erase(allTransforms.begin() + i); //rätt index?
			break;
		}
	}

	for (int i = 0; i < allMeshTransforms.size(); i++){
		if (strcmp(objName, allMeshTransforms[i]->name) == 0){
			allMeshTransforms.erase(allMeshTransforms.begin() + i); //rätt index?
			return;
		}
	}

	for (int i = 0; i < allLightTransforms.size(); i++){
		if (strcmp(objName, allLightTransforms[i]->name) == 0){
			allLightTransforms.erase(allLightTransforms.begin() + i); //rätt index?
			return;
		}
	}

	for (int i = 0; i < allCameraTransforms.size(); i++){
		if (strcmp(objName, allCameraTransforms[i]->name) == 0){
			allCameraTransforms.erase(allCameraTransforms.begin() + i); //rätt index?
			return;
		}
	}
}

void MayaLoader::MaterialAdded(MessageHeader mh, MaterialMessage *mm){
	char* materialName = mh.objectName;

	Material *tempMat; //pekar på den mesh som redan finns storad eller så blir det en helt ny
	tempMat = new Material();
	tempMat->name = mh.objectName;
	tempMat->materialData.diffuse = mm->materialData.diffuse;
	tempMat->materialData.specular = mm->materialData.specular;

	materials.push_back(tempMat);
}
void MayaLoader::MaterialChange(MessageHeader mh, MaterialMessage *mm){
	char* materialName = mh.objectName;

	Material *tempMat = nullptr;

	for (int i = 0; i < materials.size(); i++){
		if (strcmp(materialName, materials[i]->name) == 0){
			tempMat = materials[i];
			break;
		}
	}
	tempMat->EmptyVariables();
	tempMat->name = mh.objectName;
	tempMat->materialData.diffuse = mm->materialData.diffuse;
	tempMat->materialData.specular = mm->materialData.specular;
	
}
void MayaLoader::MaterialDeleted(MessageHeader mh){
	char* materialName = mh.objectName;

	for (int i = 0; i < materials.size(); i++){
		if (strcmp(materialName, materials[i]->name) == 0){
			delete materials[i];
			materials.erase(materials.begin() + i); //rätt index?
			break;
		}
	}
}

void MayaLoader::LightAdded(MessageHeader mh, LightMessage *mm){
	//char *lightName = mh.objectName;
	char* transformName = mm->transformName;
	Transform *lightTransform = nullptr; //hitta den
	Light *tempLight = nullptr;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			lightTransform = allTransforms[i];
			break;
		}
	}
	if (lightTransform == nullptr){
		printf("Hitta inte transformen");
	}

	tempLight = lightTransform->light;
	tempLight = new Light();
	tempLight->name = mh.objectName;
	tempLight->lightData = mm->lightdata;

	allLightTransforms.push_back(lightTransform); //lägger den i lightTransformsen
}
void MayaLoader::LightChange(MessageHeader mh, LightMessage *mm){
	
	char* transformName = mm->transformName;
	Transform *lightTransform = nullptr; //hitta den
	Light *tempLight = nullptr;

	for (int i = 0; i < allLightTransforms.size(); i++){
		if (strcmp(transformName, allLightTransforms[i]->name) == 0){
			lightTransform = allLightTransforms[i];
			break;
		}
	}
	if (lightTransform == nullptr){
		printf("Hitta inte transformen");
	}

	tempLight = lightTransform->light;
	tempLight->name = mh.objectName;
	tempLight->lightData = mm->lightdata;
}

void MayaLoader::CameraAdded(MessageHeader mh, CameraMessage *mm){
	char* transformName = mm->transformName;
	Transform *cameraTransform = nullptr; //hitta den
	CameraObj *tempCamera = nullptr;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			cameraTransform = allTransforms[i];
			break;
		}
	}
	if (cameraTransform == nullptr){
		printf("Hitta inte transformen");
	}

	tempCamera = cameraTransform->camera;
	tempCamera = new CameraObj();
	tempCamera->name = mh.objectName;
	tempCamera->cameraData = mm->cameraData;

	allCameraTransforms.push_back(cameraTransform);
}
void MayaLoader::CameraChange(MessageHeader mh, CameraMessage *mm){
	char* transformName = mm->transformName;
	Transform *cameraTransform = nullptr; //hitta den
	CameraObj *tempCamera = nullptr;

	for (int i = 0; i < allCameraTransforms.size(); i++){
		if (strcmp(transformName, allCameraTransforms[i]->name) == 0){
			cameraTransform = allCameraTransforms[i];
			break;
		}
	}
	if (cameraTransform == nullptr){
		printf("Hitta inte transformen");
	}

	tempCamera = cameraTransform->camera;
	tempCamera->name = mh.objectName;
	tempCamera->cameraData = mm->cameraData;
}