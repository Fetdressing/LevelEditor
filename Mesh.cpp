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
	nrVertices = meshData->nrI;
	if (vertices != nullptr)
		free(vertices);

	vertices = new Vertex[nrVertices];
	Vertex tempV;

	for (int i = 0; i < nrVertices; i++){
		int posI = meshData->indexPositions[i];
		int norI = meshData->indexNormals[i];
		int uvI = meshData->indexUVs[i];

		tempV.pos = meshData->positions[posI];
		tempV.nor = meshData->normals[norI];
		tempV.uv = meshData->uvs[uvI];

		vertices[i] = tempV;
	}
}

void Mesh::CreateVertexBuffer(){ //får skapa vertexarrayen oxå!!!!! hämta all data från meshData
	// VertexBuffer description
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(Vertex) * nrVertices;
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
	memcpy(mappedResource.pData, vertices, sizeof(Vertex) * nrVertices);
	//	Reenable GPU access to the vertex buffer data.
	gDeviceContext->Unmap(vertexBuffer, 0);
}