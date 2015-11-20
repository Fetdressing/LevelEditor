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
	Vertex *vertices = nullptr; //dessa kan inte ligga i meshData då 
	Index *indecies = nullptr;
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	//std::vector<Vertex> vertices;
	MeshData *meshData; //pekare till denna för att den har massa egna pekare nu, förstör objektet när det ska vara nya värden!
	int materialID = 0;;

	Mesh(ID3D11Device* gd, ID3D11DeviceContext* gdc){
		this->gDevice = gd; //freea dessa inte här, görs i main duuh
		this->gDeviceContext = gdc;
		materialID = 0; //default material ligger på index 0
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
	void EmptyVariables(); //används när meshen behöver bygga om värden (namn)
	void EmptyBuffersAndArrays(); //används när meshen behöver bygga om värden (vertices)
	void CreateVertices();
	void CreateIndices();
	void CreateVertexBuffer(); //dessa använder sig av den aktiva vertices och indecies
	void CreateIndexBuffer();
	void RemapVertexBuffer();


private:
	ID3D11Device* gDevice = nullptr;
	ID3D11DeviceContext* gDeviceContext = nullptr;

	HRESULT hr;
};
