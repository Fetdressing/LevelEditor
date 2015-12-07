#include "Mesh.h"
void Mesh::CreateBuffers(){
	CreateIndices();
	CreateVertices();

	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::EmptyVariables(){
	delete(name);
	delete(materialName);
	delete(meshData);
}

void Mesh::EmptyBuffers() {
	vertexBuffer->Release();
	indexBuffer->Release();
}
//
//void Mesh::EmptyBuffersAndArrays(){
//	delete(meshData);
//
//	vertexBuffer->Release();
//	indexBuffer->Release();
//}

void Mesh::CreateIndices(){
	nrIndecies = meshData->nrI;
	if (indecies != nullptr)
		free(indecies);

	indecies = new Index[nrIndecies];
	Index tempI;
	for (int i = 0; i < nrIndecies; i++){
		tempI.posI = meshData->indexPositions[i];
		tempI.norI = meshData->indexNormals[i];
		tempI.uvI = meshData->indexUVs[i];
		indecies[i] = tempI;
	}
}

void Mesh::CreateVertices(){
	nrVertices = meshData->nrPos;
	if (vertices != nullptr)
		free(vertices);

	vertices = new Vertex[nrIndecies];
	Vertex tempV;

	//for (int i = 0; i < nrIndecies; i++){
	//	
	//	tempV.pos = meshData->positions[indecies[i].posI];
	//	tempV.nor = meshData->normals[indecies[i].norI];
	//	tempV.uv = meshData->uvs[indecies[i].uvI];

	//	vertices[i] = tempV;
	//}

	for (int i = 0; i < nrIndecies; i = i + 3) {
		tempV.pos = meshData->positions[indecies[i+2].posI];
		tempV.nor = meshData->normals[indecies[i+2].norI];
		tempV.uv = meshData->uvs[indecies[i+2].uvI];

		vertices[i] = tempV;

		tempV.pos = meshData->positions[indecies[i+1].posI];
		tempV.nor = meshData->normals[indecies[i+1].norI];
		tempV.uv = meshData->uvs[indecies[i+1].uvI];

		vertices[i + 1] = tempV;

		tempV.pos = meshData->positions[indecies[i].posI];
		tempV.nor = meshData->normals[indecies[i].norI];
		tempV.uv = meshData->uvs[indecies[i].uvI];

		vertices[i + 2] = tempV;
	}
}

void Mesh::CreateVertexBuffer(){ //får skapa vertexarrayen oxå!!!!! hämta all data från meshData
	// VertexBuffer description
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(Vertex) * nrIndecies;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

}

void Mesh::CreateIndexBuffer(){

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Index) * nrIndecies;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Define the resource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indecies;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer with the device.
	hr = gDevice->CreateBuffer(&bufferDesc, &InitData, &indexBuffer);
	

}

void Mesh::RemapVertexBuffer(){
	CreateIndices(); //antar jag behöver dessa här!!
	CreateVertices();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gDeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, vertices, sizeof(Vertex) * nrIndecies);
	//	Reenable GPU access to the vertex buffer data.
	gDeviceContext->Unmap(vertexBuffer, 0);
}