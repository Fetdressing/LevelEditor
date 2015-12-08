#include "MayaLoader.h"

Mutex mutexInfo("__info_Mutex__");
MayaLoader::MayaLoader(ID3D11Device* gd, ID3D11DeviceContext* gdc, UINT screenWidth, UINT screenHeight){
	this->gDevice = gd;
	this->gDeviceContext = gdc;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	CreateFileMaps(1024 * 1024 * 10);
	InitVariables();
	
	Material *defaultMaterial = new Material(gDevice, gDeviceContext);
	materials.push_back(defaultMaterial); //lägg till default material, viktigt den ligger på första platsen

}
MayaLoader::~MayaLoader(){
	
	UnmapViewOfFile((LPCVOID)mMessageData);
	CloseHandle(hMessageFileMap);

	UnmapViewOfFile((LPCVOID)mInfoData);
	CloseHandle(hInfoFileMap);
	
	//free(transformMessage); //görs av transformens destruktor
	//free(meshMessage);
	//free(cameraMessage);
	//free(lightMessage);
	//free(materialMessage);

}

void MayaLoader::CreateFileMaps(unsigned int messageFilemapSize){
	//mutexInfo.Unlock(); //kanske inte den smartaste idéen?
	//messagefilemap
	mSize = messageFilemapSize;
	hMessageFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		mSize,
		(LPCWSTR) "Global\\messageFileMap");

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
		(LPCWSTR) "Global\\infoFileMap");

	mInfoData = MapViewOfFile(hInfoFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (hInfoFileMap == NULL)
	{
		cout << "Couldn't create infofilemap\n";
	}
	//FilemapInfo fmInfo;
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		cout << "Infofilemap exists, you get an handle to it!\n";
	}
	else //first, sätter de första värdena på filemapinfon
	{ 
		cout << "Creating new infofilemap\n";


	}
	

	SetFilemapInfoValues(0, 0, 10, mSize); //storar de i filemapen oxå! sätt negativa värden om man inte vill nått värde ska ändras :)

}
void MayaLoader::InitVariables() {
	D3D11_BUFFER_DESC cameraBufferDesc;
	memset(&cameraBufferDesc, 0, sizeof(cameraBufferDesc));
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	cameraBufferDesc.ByteWidth = sizeof(CameraCBufferData);
	gDevice->CreateBuffer(&cameraBufferDesc, NULL, &cDefaultCameraConstantBuffer);

	XMStoreFloat4x4(&defaultCameraCBufferData.view, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&defaultCameraCBufferData.projection, XMMatrixTranspose(XMMatrixIdentity()));
	gDeviceContext->UpdateSubresource(cDefaultCameraConstantBuffer, 0, NULL, &defaultCameraCBufferData, 0, 0); //default buffer
	//fpsCam.SetLens(0.25f*3.14f, screenWidth / screenHeight, 1.0f, 1000.0f);
}

void MayaLoader::SetFilemapInfoValues(size_t headPlacement, size_t tailPlacement, size_t nonAccessMemoryPlacement, size_t messageFileMapTotalSize){
	
	while (mutexInfo.Lock(1000) == false) Sleep(10); //kommer kalla raden under massa onödiga gånger :s
	
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	if (headPlacement >= 0)
		fileMapInfo.head_ByteOffset = headPlacement;
	if (tailPlacement >= 0)
		fileMapInfo.tail_ByteOffset = tailPlacement;
	if (nonAccessMemoryPlacement >= 0)
		fileMapInfo.non_accessmemoryOffset = nonAccessMemoryPlacement;
	if (messageFileMapTotalSize > 0)
		fileMapInfo.messageFilemap_Size = messageFileMapTotalSize;
	
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();
}

void MayaLoader::DrawScene(){
	UINT32 vertexSize2 = sizeof(float) * 8;
	UINT32 offset2 = 0;
	//set rätt constantbuffers, ljus, kamera och material stuff!
	UpdateCameraValues(); //updaterar oxå camera cbuffern

	for (int i = 0; i < allMeshTransforms.size(); i++)
	{
		Mesh *currMesh = allMeshTransforms[i]->mesh;
		gDeviceContext->IASetVertexBuffers(0, 1, &currMesh->vertexBuffer, &vertexSize2, &offset2);
		gDeviceContext->IASetIndexBuffer(currMesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		try{
			gDeviceContext->PSSetConstantBuffers(1, 0, &allMeshTransforms[i]->mesh->material->materialCbuffer); //materialID är satt till 0 i början, dvs default material
		}
		catch (...){ //gick inte assigna det materialet (inte skapat ännu förmodligen), använd default istället
			gDeviceContext->PSSetConstantBuffers(1, 0, &materials[0]->materialCbuffer); //defaultmat
		}
		//transformdata ligger på plats 0, material på 1, osv
		//set transformcbufferns värden, updatesubresource
		allMeshTransforms[i]->UpdateCBuffer(); //slå först ihop med parentens värden innan vi updaterar cbuffern
		gDeviceContext->Draw(allMeshTransforms[i]->mesh->nrIndecies, 0);
		
		//gDeviceContext->DrawIndexed(currMesh->nrIndecies, 0, 0);
	}
}

void MayaLoader::TryReadAMessage(){
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo)); //hämta filemapinfo datan om jag har fått mutexInfo till den

	if (localTail != fileMapInfo.head_ByteOffset){ //sålänge consumern inte har hunnit till headern
		//KAN LÄSA!!
		if ((localTail + sizeof(MessageHeader)) <= mSize){ //headern ligger på denna sidan!
			headerDidFit = true;
			memcpy(&messageHeader, (unsigned char*)mMessageData + localTail, sizeof(MessageHeader)); //läs headern som vanligt
		}
		else{ //headern ligger på andra sidan, headern får inte plats
			headerDidFit = false;
			memcpy(&messageHeader, (unsigned char*)mMessageData, sizeof(MessageHeader)); //läs headern i början på filen
			localTail = 0; //flytta över hela meddelandet till andra sidan
		}
		
		switch (messageHeader.nodeType){
			case 0:
				printf("Skumt ID");
				break;
			case 1:
			ReadMesh(messageHeader.messageType);
				break;
			case 2:				
				ReadTransform(messageHeader.messageType);
				break;
			case 3:
				ReadCamera(messageHeader.messageType);				
				break;
			case 4:
				ReadLight(messageHeader.messageType);				
				break;
			case 5:
				ReadMaterial(messageHeader.messageType);
				break;
			default:
				printf("Invalid message ID");
		}

	}

}

void MayaLoader::ReadTransform(int i)
{
	//LÄS MEDDELANDE OCH HEADER
	//får bara headern plats eller får oxå meddelandet plats?
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		transformMessage = (TransformMessage*)malloc(transformMessage_MaxSize);
		if (messageHeader.msgConfig == 1){ //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(transformMessage, (unsigned char*)mMessageData, messageHeader.byteSize); //läser i början på filen utan nån offset
			localTail = messageHeader.byteSize + messageHeader.bytePadding;
		}
		else{ //meddelandet får plats!!
			memcpy(transformMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal + localTail;
		}
		
	}
	else{ //headern ligger på andra sidan, headern får inte plats
		if ((localTail + messageHeader.byteSize) > mSize){ //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else{ //annars bara läs den rakt av från där denna redan är placerad
			transformMessage = (TransformMessage*)malloc(transformMessage_MaxSize);
			memcpy(transformMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal;
		}
		
	}

	if (i == 1)
	{
		TransformAdded(messageHeader, transformMessage);
	}
	else if (i == 2)
	{
		TransformChange(messageHeader, transformMessage); //tar hand om den aktualla meshen
	}
	else if (i == 3)
	{
		TransformDeleted(messageHeader);
	}


	//flytta tailen
	while (mutexInfo.Lock(1000) == false) Sleep(10);
	cout << "Move tail!!!!!" << localTail << "\n";
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = localTail;
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();


}
void MayaLoader::ReadMaterial(int i)
{
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		materialMessage = (MaterialMessage*)malloc(materialMessage_MaxSize);
		if (messageHeader.msgConfig == 1) { //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(materialMessage, (unsigned char*)mMessageData, messageHeader.byteSize); //läser i början på filen utan nån offset
			localTail = messageHeader.byteSize + messageHeader.bytePadding;
		}
		else { //meddelandet får plats!!
			memcpy(materialMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal + localTail;
		}

	}
	else { //headern ligger på andra sidan, headern får inte plats
		   //messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((localTail + messageHeader.byteSize) > mSize) { //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else { //annars bara läs den rakt av från där denna redan är placerad
			materialMessage = (MaterialMessage*)malloc(materialMessage_MaxSize);
			memcpy(materialMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal;
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
	while (mutexInfo.Lock(1000) == false) Sleep(10);
	cout << "Move tail!!!!!" << localTail << "\n";
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = localTail;
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadMesh(int i)
{
	//LÄS MEDDELANDE OCH HEADER
	//får bara headern plats eller får oxå meddelandet plats?
	if (headerDidFit == true) { //headern ligger som vanligt, alltså där man är, headern får plats
		meshMessage = (MeshMessage*)malloc(meshMessage_MaxSize);
		if (messageHeader.msgConfig == 1) { //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			ReadMeshData(0);
			localTail = messageHeader.byteSize + messageHeader.bytePadding;
		}
		else { //meddelandet får plats!!
			ReadMeshData(localTail + sizeof(MessageHeader));
			localTail = messageHeader.byteTotal + localTail;
		}

	}
	else { //headern ligger på andra sidan, headern får inte plats
		   //messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((localTail + messageHeader.byteSize) > mSize) //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
		{ 
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else //annars bara läs den rakt av från där denna redan är placerad
		{
			meshMessage = (MeshMessage*)malloc(meshMessage_MaxSize);
			ReadMeshData(sizeof(MessageHeader));
			localTail = messageHeader.byteTotal;
		}

	}

	if (i == 1){
		MeshAdded(messageHeader, meshMessage);
	}
	else if (i == 2){
		MeshChange(messageHeader, meshMessage); //tar hand om den aktualla meshen
	}
	//flytta tailen
	while (mutexInfo.Lock(1000) == false) Sleep(10);
	cout << "Move tail!!!!!" << localTail << "\n";
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = localTail;
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadMeshData(size_t offSetStart)
{
	meshMessage->meshData = new MeshData();

	memcpy(meshMessage->objectName, (unsigned char*)mMessageData + offSetStart, sizeof(meshMessage->objectName));
	UINT offset = (sizeof(char) * 100);
	memcpy(meshMessage->transformName, (unsigned char*)mMessageData + offSetStart + offset, sizeof(meshMessage->transformName));
	offset += (sizeof(char) * 100);
	memcpy(meshMessage->meshData, (unsigned char*)mMessageData + offSetStart + offset, sizeof(int) * 5);
	offset += sizeof(int) * 5; //7

	//allokera minne till att variabler!
	meshMessage->meshData->positions = new Float3[meshMessage->meshData->nrPos];
	meshMessage->meshData->normals = new Float3[meshMessage->meshData->nrNor];
	meshMessage->meshData->uvs = new Float2[meshMessage->meshData->nrUV];

	meshMessage->meshData->indexPositions = new int[meshMessage->meshData->nrI];
	meshMessage->meshData->indexNormals = new int[meshMessage->meshData->nrI];
	meshMessage->meshData->indexUVs = new int[meshMessage->meshData->nrI];
	meshMessage->meshData->trianglesPerFace = new int[meshMessage->meshData->triangleCount];

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

}
void MayaLoader::ReadLight(int i)
{
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		lightMessage = (LightMessage*)malloc(lightMessage_MaxSize);
		if (messageHeader.msgConfig == 1) { //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(lightMessage, (unsigned char*)mMessageData, messageHeader.byteSize); //läser i början på filen utan nån offset
			localTail = messageHeader.byteSize + messageHeader.bytePadding;
		}
		else { //meddelandet får plats!!
			memcpy(lightMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal + localTail;
		}

	}
	else { //headern ligger på andra sidan, headern får inte plats
		   //messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((localTail + messageHeader.byteSize) > mSize) { //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else { //annars bara läs den rakt av från där denna redan är placerad
			lightMessage = (LightMessage*)malloc(lightMessage_MaxSize);
			memcpy(lightMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal;
		}

	}

	if (i == 1){
		LightAdded(messageHeader, lightMessage);
	}
	else if (i == 2){
		LightChange(messageHeader, lightMessage); //tar hand om den aktualla meshen
	}
	
	//flytta tailen
	while (mutexInfo.Lock(1000) == false) Sleep(10);
	cout << "Move tail!!!!!" << localTail << "\n";
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = localTail;
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}
void MayaLoader::ReadCamera(int i){
	if (headerDidFit == true){ //headern ligger som vanligt, alltså där man är, headern får plats
		cameraMessage = (CameraMessage*)malloc(cameraMessage_MaxSize);
		if (messageHeader.msgConfig == 1) { //meddelandet får däremot inte plats -> meddelandet är skickat till andra sidan
			memcpy(cameraMessage, (unsigned char*)mMessageData, messageHeader.byteSize); //läser i början på filen utan nån offset
			localTail = messageHeader.byteSize + messageHeader.bytePadding;
		}
		else { //meddelandet får plats!!
			memcpy(cameraMessage, (unsigned char*)mMessageData + localTail + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal + localTail;
		}

	}
	else { //headern ligger på andra sidan, headern får inte plats
		   //messageFile.message = (char*)malloc(messageFile.header.byteSize - sizeof(MessageHeader)); //allokera minne till den lokala variablen att hålla meddelandet

		if ((localTail + messageHeader.byteSize) > mSize) { //meddelandet får inte plats innan filemapen tar slut -> meddelandet är skickat till andra sidan
			cout << "Filemap too small, a message might be to big for the entire filemap";
		}
		else { //annars bara läs den rakt av från där denna redan är placerad
			cameraMessage = (CameraMessage*)malloc(cameraMessage_MaxSize);
			memcpy(cameraMessage, (unsigned char*)mMessageData + sizeof(MessageHeader), messageHeader.byteSize);
			localTail = messageHeader.byteTotal;
		}

	}

	if (i == 1){
		CameraAdded(messageHeader, cameraMessage);
	}
	else if (i == 2){
		CameraChange(messageHeader, cameraMessage); //tar hand om den aktualla meshen
	}

	//flytta tailen
	while (mutexInfo.Lock(1000) == false) Sleep(10);
	cout << "Move tail!!!!!" << localTail << "\n";
	memcpy(&fileMapInfo, (unsigned char*)mInfoData, sizeof(FilemapInfo));
	fileMapInfo.tail_ByteOffset = localTail;
	memcpy((unsigned char*)mInfoData, &fileMapInfo, sizeof(FilemapInfo));
	mutexInfo.Unlock();

}

void MayaLoader::MeshAdded(MessageHeader mh, MeshMessage *mm)
{ //material måste alltid komma före meshes!!
	char* meshName = mm->objectName;
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
		printf("Hittade inte transformen");
	}
	else
	{
		
		meshTransform->mesh = new Mesh(gDevice, gDeviceContext);
		activeMesh = meshTransform->mesh;

		activeMesh->name = mm->objectName;
		activeMesh->transformName = mm->transformName;
		activeMesh->materialName = mm->materialName;
		activeMesh->meshData = mm->meshData;

		activeMesh->material = materials[0]; //default material
		//if (activeMesh->material->name != nullptr)
		//{
		//	for (int i = 0; i < materials.size(); i++) {
		//		if (strcmp(mm->materialName, materials[i]->name) == 0) {
		//			activeMesh->material = materials[i];
		//			break;
		//		}
		//	}
		//}

		activeMesh->CreateBuffers();
		allMeshTransforms.push_back(meshTransform); //skickar in denna transform i allMeshTransforms oxå!! så den kommer vara refererad i båda vektorerna
	}
	
}
void MayaLoader::MeshChange(MessageHeader mh, MeshMessage *mm)
{ //MÅSTE HA TRANSFORMEN FÖRST, SEN SKAPA ETT MESH OBJEKT I TRANSFORMEN
	char* meshName = mm->objectName;
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
		printf("Hittade inte transformen");
	}
	else
	{
		activeMesh = meshTransform->mesh;
		activeMesh->EmptyBuffers(); //ska denna göras? man remappar ju dem -> kolla upp!!
		activeMesh->EmptyVariables(); //viktigt att göra dessa, annars kommer variablerna gå lost utan referens!!

		activeMesh->name = mm->objectName;
		activeMesh->materialName = mm->materialName;
		activeMesh->meshData = mm->meshData;

		activeMesh->material = materials[0]; //default material
		for (int i = 0; i < materials.size(); i++) {
			if (strcmp(mm->materialName, materials[i]->name) == 0) {
				activeMesh->material = materials[i];
				break;
			}
		}
		//activeMesh->meshData.nrVerts = mh.nrVerts;
		//activeMesh->meshData.nrIndecies = mh.nrIndecies;

		//activeMesh->meshData.vertices = mm->meshData.vertices;
		//activeMesh->meshData.indecies = mm->meshData.indecies;

		activeMesh->RemapVertexBuffer();
	}
}

void MayaLoader::TransformAdded(MessageHeader mh, TransformMessage *mm)
{
	Transform *transform = new Transform(gDevice, gDeviceContext); //hitta den

	transform->name = mm->objectName;
	transform->parentName = mm->parentName;
	transform->transformData = mm->transformData;

	if (transform->parentName[0] != 0){ //namnet är inte tomt -> den har en parent, så hitta den
		for (int i = 0; i < allTransforms.size(); i++) {
			if (strcmp(transform->parentName, allTransforms[i]->name) == 0) {
				transform->parent = allTransforms[i];
				break;
			}
		}
	}	

	allTransforms.push_back(transform);

	//test!!!!
	fileHandler = new FileHandler();
	const char* skitName = "Hej" + 0;
	fileHandler->SaveScene(MAX_NAME_SIZE, (char*)skitName,
		materials,
		allTransforms,
		allMeshTransforms,
		allLightTransforms);
	//test!!!!
}
void MayaLoader::TransformChange(MessageHeader mh, TransformMessage *mm)
{
	char* transformName = mm->objectName;
	Transform *transform = nullptr;

	for (int i = 0; i < allTransforms.size(); i++){
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			transform = allTransforms[i];
			break;
		}
	}

	if (transform != nullptr)
	{
		transform->EmptyVariables();

		transform->name = transformName;
		transform->parentName = mm->parentName;
		transform->transformData = mm->transformData;

		if (transform->parentName[0] != '0') { //namnet är inte tomt -> den har en parent, så hitta den
			for (int i = 0; i < allTransforms.size(); i++) {
				if (strcmp(transform->parentName, allTransforms[i]->name) == 0) {
					transform->parent = allTransforms[i];
					break;
				}
			}
		}
	}

}
void MayaLoader::TransformDeleted(MessageHeader mh){ //ta bort från alla vektorer!!! inte bara allTransforms!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//char* objName = messageHeader.objectName;

	//for (int i = 0; i < allTransforms.size(); i++){
	//	if (strcmp(objName, allTransforms[i]->name) == 0){
	//		delete allTransforms[i];
	//		allTransforms.erase(allTransforms.begin() + i); //rätt index?
	//		break;
	//	}
	//}

	//for (int i = 0; i < allMeshTransforms.size(); i++){
	//	if (strcmp(objName, allMeshTransforms[i]->name) == 0){
	//		allMeshTransforms.erase(allMeshTransforms.begin() + i); //rätt index?
	//		return;
	//	}
	//}

	//for (int i = 0; i < allLightTransforms.size(); i++){
	//	if (strcmp(objName, allLightTransforms[i]->name) == 0){
	//		allLightTransforms.erase(allLightTransforms.begin() + i); //rätt index?
	//		return;
	//	}
	//}

	//for (int i = 0; i < allCameraTransforms.size(); i++){
	//	if (strcmp(objName, allCameraTransforms[i]->name) == 0){
	//		allCameraTransforms.erase(allCameraTransforms.begin() + i); //rätt index?
	//		return;
	//	}
	//}
}

void MayaLoader::MaterialAdded(MessageHeader mh, MaterialMessage *mm)
{
	//char* materialName = mm->objectName;

	Material *tempMat; //pekar på den mesh som redan finns storad eller så blir det en helt ny
	tempMat = new Material(gDevice, gDeviceContext);
	tempMat->CreateCBuffer();
	tempMat->name = mm->objectName;
	tempMat->materialData.diffuse = mm->materialData.diffuse;
	tempMat->materialData.specular = mm->materialData.specular;	

	tempMat->UpdateCBuffer(); //lägger in de nya värdena i cbuffern
	materials.push_back(tempMat);
}
void MayaLoader::MaterialChange(MessageHeader mh, MaterialMessage *mm)
{
	char* materialName = mm->objectName;

	Material *tempMat = nullptr;

	for (int i = 0; i < materials.size(); i++){
		if (strcmp(materialName, materials[i]->name) == 0){
			tempMat = materials[i];
			break;
		}
	}
	tempMat->EmptyVariables(); //viktigt så att vi inte tappar referens till något som vi ska ta bort
	tempMat->name = mm->objectName;
	tempMat->materialData.diffuse = mm->materialData.diffuse;
	tempMat->materialData.specular = mm->materialData.specular;
	tempMat->UpdateCBuffer();
	
}
void MayaLoader::MaterialDeleted(MessageHeader mh)
{
	//char* materialName = mh.objectName;

	//for (int i = 0; i < materials.size(); i++){
	//	if (strcmp(materialName, materials[i]->name) == 0){
	//		delete materials[i];
	//		materials.erase(materials.begin() + i); //rätt index?
	//		break;
	//	}
	//}
}

void MayaLoader::LightAdded(MessageHeader mh, LightMessage *mm)
{
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
		printf("Hittade inte transformen");
	}
	else
	{
		lightTransform->light = new Light(gDevice, gDeviceContext);
		tempLight = lightTransform->light;

		tempLight->transform = lightTransform;
		tempLight->CreateCBuffer();

		tempLight->name = mm->objectName;
		tempLight->lightData = mm->lightdata;
		tempLight->UpdateCBuffer();

		allLightTransforms.push_back(lightTransform); //lägger den i lightTransformsen
	}
}
void MayaLoader::LightChange(MessageHeader mh, LightMessage *mm)
{
	
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
	else
	{
		tempLight = lightTransform->light;
		tempLight->EmptyVariables();

		tempLight->name = mm->objectName;
		tempLight->lightData = mm->lightdata;
		tempLight->UpdateCBuffer();
	}
}

void MayaLoader::CameraAdded(MessageHeader mh, CameraMessage *mm)
{
	char* transformName = mm->transformName;
	Transform *cameraTransform = nullptr; //hitta den
	CameraObj *tempCamera = nullptr;

	for (int i = 0; i < allTransforms.size(); i++)
	{
		if (strcmp(transformName, allTransforms[i]->name) == 0){
			cameraTransform = allTransforms[i];
			break;
		}
	}
	if (cameraTransform == nullptr)
	{
		printf("Hittade inte transformen");
	}
	else
	{
		cameraTransform->camera = new CameraObj(gDevice, gDeviceContext);
		tempCamera = cameraTransform->camera;

		tempCamera->CreateCBuffer();
		
		tempCamera->name = mm->objectName;
		tempCamera->transform = cameraTransform;
		tempCamera->cameraData = mm->cameraData;
		tempCamera->UpdateCBuffer(screenWidth, screenHeight);

		currentCameraTransform = cameraTransform; //denna är temporär, ska bara ske vid CameraSwitch funktionen

		allCameraTransforms.push_back(cameraTransform);
	}
}
void MayaLoader::CameraChange(MessageHeader mh, CameraMessage *mm)
{
	char* transformName = mm->transformName;
	Transform *cameraTransform = nullptr; //hitta den för att hitta vilken camera den syftar på
	CameraObj *tempCamera = nullptr;

	for (int i = 0; i < allCameraTransforms.size(); i++){
		if (strcmp(transformName, allCameraTransforms[i]->name) == 0){
			cameraTransform = allCameraTransforms[i];
			break;
		}
	}
	if (cameraTransform == nullptr){ //ha kvar pekaren på den gamla transformen!
		printf("Hittade inte transformen");
	}
	else
	{
		tempCamera->EmptyVariables();
	
		//tempCamera->transform = cameraTransform; //ge kameran reference till transformen, bara vid starten? den byter väl inte transform?

		tempCamera->name = mm->objectName;
		tempCamera = cameraTransform->camera;
		tempCamera->cameraData = mm->cameraData;
		tempCamera->UpdateCBuffer(screenWidth, screenHeight);
	}
}
void MayaLoader::CameraSwitch(MessageHeader mh, CameraMessage *mm) 
{
	char* newActiveCameraTransformName = mm->transformName;

	for (int i = 0; i < allCameraTransforms.size(); i++) {
		if (strcmp(newActiveCameraTransformName, allCameraTransforms[i]->name) == 0) {
			currentCameraTransform = allCameraTransforms[i];
			break;
		}
	}
}

bool MayaLoader::UpdateCameraValues()
{
	if (currentCameraTransform != nullptr)
	{
		currentCameraTransform->camera->UpdateCBuffer(screenWidth, screenHeight);

		//gDeviceContext->UpdateSubresource(cCameraConstantBuffer, 0, NULL, &cameraCBufferData, 0, 0);
		gDeviceContext->VSSetConstantBuffers(10, 1, &currentCameraTransform->camera->cameraCbuffer);

		return true;
	}
	gDeviceContext->VSSetConstantBuffers(10, 1, &cDefaultCameraConstantBuffer); //kör på default identitesmatriser annars
	return false;
}