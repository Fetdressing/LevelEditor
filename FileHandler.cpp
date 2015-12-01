#include "FileHandler.h"

FileHandler::FileHandler()
{

}
FileHandler::~FileHandler(){}


void FileHandler::SaveScene(int maxNameSize, char* sceneName,
	vector<Material*> &materials,
	vector<Transform*> &allTransforms,
	vector<Transform*> &allMeshTransforms,
	vector<Transform*> &allLightTransforms)
{
	max_Name_Size = maxNameSize;
	int sceneNameSize = CorrectName(sceneName);
	int nrMats = materials.size();
	int nrTransforms = allTransforms.size();
	int nrMeshes = allMeshTransforms.size();
	int nrLights = allLightTransforms.size();


	ofs.open("test.drm", ofstream::out | ofstream::binary);
	if (ofs.is_open() == true) {
		ofs.clear();

		SaveMainHeader(sceneNameSize, sceneName, nrMats, nrTransforms, nrMeshes, nrLights);

		SaveMaterials(nrMats, materials); //måste vara först!
		SaveTransforms(nrTransforms, allTransforms);
		SaveMeshes(nrMeshes, allMeshTransforms);
		SaveLights(nrLights, allLightTransforms);
		
		ofs.close();
	}
}

void FileHandler::SaveMainHeader(int sceneNameSize, char* sceneName, int nrMats, int nrTransforms, int nrMeshes, int nrLights)
{
	ofs.write((char*)&sceneNameSize, sizeof(int));
	ofs.write(sceneName, sizeof(char) * sceneNameSize);

	ofs.write((char*)&nrMats, sizeof(int));
	ofs.write((char*)&nrTransforms, sizeof(int));
	ofs.write((char*)&nrMeshes, sizeof(int));
	ofs.write((char*)&nrLights, sizeof(int));	
}

void FileHandler::SaveTransforms(int nrTransforms, vector<Transform*> &allTransforms)
{
	for (int i = 0; i < nrTransforms; i++) {
		char* parentName = allTransforms[i]->parentName;
		char* transformName = allTransforms[i]->name;
		int parentNameSize = CorrectName(parentName); //biter av vid nullterminator och returnerar längden på texten
		int transformNameSize = CorrectName(transformName);

		ofs.write((char*)&parentNameSize, sizeof(int));
		ofs.write((char*)&transformNameSize, sizeof(int));
		ofs.write(parentName, sizeof(char) * parentNameSize);
		ofs.write(transformName, sizeof(char) * transformNameSize);

		if (transformNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(transformName);
		}
		if (parentNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(parentName);
		}
		
		ofs.write((char*)&allTransforms[i]->transformData.pos, sizeof(allTransforms[i]->transformData.pos));
		ofs.write((char*)&allTransforms[i]->transformData.rot, sizeof(allTransforms[i]->transformData.rot));
		ofs.write((char*)&allTransforms[i]->transformData.scale, sizeof(allTransforms[i]->transformData.scale));
	}
}

void FileHandler::SaveMeshes(int nrMeshes, vector<Transform*> &allMeshTransforms)
{
	for (int i = 0; i < nrMeshes; i++) {

		//headerSTART****
		char* transformName = allMeshTransforms[i]->mesh->transformName; //hade nog egentligen kunnat använda transformnamnet då det är samma, men consistency u know
		char* meshName = allMeshTransforms[i]->mesh->name;
		int transformNameSize = CorrectName(transformName);
		int meshNameSize = CorrectName(meshName); //biter av vid nullterminator och returnerar längden på texten

		ofs.write((char*)&transformNameSize, sizeof(int));
		ofs.write((char*)&meshNameSize, sizeof(int));
		ofs.write(transformName, sizeof(char) * transformNameSize);
		ofs.write(meshName, sizeof(char) * meshNameSize);

		if (transformNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(transformName);
		}
		if (meshNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(meshName);
		}

		/*ofs.write((char*)&allMeshTransforms[i]->mesh->nrVertices, sizeof(int));
		ofs.write((char*)&allMeshTransforms[i]->mesh->nrIndecies, sizeof(int));
		*/
		char* materialName = allMeshTransforms[i]->mesh->materialName;
		int materialNameSize = CorrectName(materialName);

		ofs.write((char*)&materialNameSize, sizeof(int));
		ofs.write(materialName, sizeof(char) * materialNameSize);

		if (materialNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(materialName);
		}
		//headerEND****

		//messageSTART****
		ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrPos, sizeof(int));
		ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrNor, sizeof(int));
		ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrUV, sizeof(int));
		ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->nrI, sizeof(int));
		ofs.write((char*)&allMeshTransforms[i]->mesh->meshData->triangleCount, sizeof(int));

		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->positions, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrPos); //?? pekare till float3s
		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->normals, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrNor);
		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->uvs, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrUV);

		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexPositions, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrI);
		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexNormals, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrI);
		ofs.write((char*)allMeshTransforms[i]->mesh->meshData->indexUVs, sizeof(Float3) * allMeshTransforms[i]->mesh->meshData->nrI);
		//messageEND****
	}
}

void FileHandler::SaveMaterials(int nrMats, vector<Material*> &materials)
{
	for (int i = 1; i < nrMats; i++) { //läs inte default materialet!
		char* materialName = materials[i]->name;
		int materialNameSize = CorrectName(materialName);

		ofs.write((char*)&materialNameSize, sizeof(int));
		ofs.write(materialName, sizeof(char) * materialNameSize);

		if (materialNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(materialName);
		}
	}
}

void FileHandler::SaveLights(int nrLights, vector<Transform*> &allLightTransforms)
{
	for (int i = 0; i < nrLights; i++) {
		char* transformName = allLightTransforms[i]->name;
		char* lightName = allLightTransforms[i]->light->name;
		int transformNameSize = CorrectName(transformName);
		int lightNameSize = CorrectName(lightName);

		ofs.write((char*)&transformNameSize, sizeof(int));
		ofs.write((char*)&lightNameSize, sizeof(int));
		ofs.write(transformName, sizeof(char) * transformNameSize);
		ofs.write(lightName, sizeof(char) * lightNameSize);

		if (transformNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(transformName);
		}
		if (lightNameSize == 0) //den är dynamiskt allokerad!
		{
			delete(lightName);
		}
		//lightdata!!

	}
}


void FileHandler::LoadScene()
{

}


int FileHandler::CorrectName(char *&referenceName) { //kör tills nollbyten och biter av resterande chars
	char *tempName = nullptr;
	int nameSize = 0;
	if (referenceName != nullptr)
	{
		for (int i = 0; i < max_Name_Size; i++) {
			if (referenceName[i] == 0) { //nullterminator!!!!!!!!!!!
				break;
			}
			nameSize++; //här ??
		}

		tempName = new char[nameSize];
		for (int i = 0; i < nameSize; i++) {
			tempName[i] = referenceName[i];
		}

		//delete(referenceName); haha arrayen som den pekar på är statisk är ju fan statisk!
		referenceName = new char[nameSize];
		referenceName = tempName;
		return nameSize;
	}
	else
	{
		return 0;
	}
}