#pragma once
#ifndef MESH_H
#define MESH_H
#endif

//CRUCIAL
#include <windows.h>
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <SimpleMath.h>
#include <string>

using namespace DirectX;
using namespace DirectX::SimpleMath;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "ObjectData.h"

class Mesh{
public:
	char* name; //det riktiga namnet finns i Transformen
	char* transformName;
	//XMMATRIX pos, rot, scale, world;
	int nrVertices;
	int nrIndecies;
	Vertex *vertices = nullptr; //dessa kan inte ligga i meshData d� 
	Index *indecies = nullptr;
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	//std::vector<Vertex> vertices;
	MeshData *meshData; //pekare till denna f�r att den har massa egna pekare nu, f�rst�r objektet n�r det ska vara nya v�rden!
	int materialID = 0;;

	Mesh(ID3D11Device* gd, ID3D11DeviceContext* gdc){
		this->gDevice = gd; //freea dessa inte h�r, g�rs i main duuh
		this->gDeviceContext = gdc;
		materialID = 0; //default material ligger p� index 0
	}
	~Mesh(){
		vertexBuffer->Release();
		indexBuffer->Release();

		delete(name);
		delete(vertices);
		delete(indecies);
		delete(meshData);
	}
	void CreateBuffers();
	void EmptyVariables(); //anv�nds n�r meshen beh�ver bygga om v�rden (namn)
	void EmptyBuffersAndArrays(); //anv�nds n�r meshen beh�ver bygga om v�rden (vertices)
	void CreateVertices();
	void CreateIndices();
	void CreateVertexBuffer(); //dessa anv�nder sig av den aktiva vertices och indecies
	void CreateIndexBuffer();
	void RemapVertexBuffer();


private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;

	HRESULT hr;
};
