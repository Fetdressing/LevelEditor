#pragma once
#ifndef OBJECTDATA_H
#define OBJECTDATA_H
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

struct Float2{
	float u, v;
	Float2(){}
	Float2(float fu, float fv){
		u = fu;
		v = fv;
	}

	void Set(float fu, float fv){
		u = fu;
		v = fv;
	}
};

struct Float3{
	float x, y, z;
	Float3(){
		x = 0;
		y = 0;
		z = 0;
	}
	Float3(float fx, float fy, float fz){
		x = fx;
		y = fy;
		z = fz;
	}

	void Set(float fx, float fy, float fz){
		x = fx;
		y = fy;
		z = fz;
	}
};

struct Float4{
	float x, y, z, w;
	Float4(){}
	Float4(float fx, float fy, float fz, float fw){
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}

	void Set(float fx, float fy, float fz, float fw){
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
};

struct Int3{
	int x, y, z;
	Int3(){}
	Int3(int fx, int fy, int fz){
		x = fx;
		y = fy;
		z = fz;
	}

	void Set(int fx, int fy, int fz){
		x = fx;
		y = fy;
		z = fz;
	}
};
struct Int2{
	int u, v;
	Int2(){}
	Int2(int fu, int fv){
		u = fu;
		v = fv;
	}

	void Set(int fu, int fv){
		u = fu;
		v = fv;
	}
};

struct Vertex{
	Float3 pos, nor;
	Float2 uv;

	Vertex(){}
	~Vertex(){}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, float cu, float cv){
		pos.Set(px, py, pz);
		nor.Set(nx, ny, nz);
		uv.Set(cu, cv);
	}
	
};
struct Index{
	int posI, norI, uvI;

	Index(){}
	~Index(){}
	Index(int p, int n, int u){
		posI = p;
		norI = n;
		uvI = u;
	}

};

//
//struct Mesh{
//	char* meshName; //det riktiga namnet finns i Transformen
//	//XMMATRIX pos, rot, scale, world;
//	ID3D11Buffer *vertexBuffer;
//	ID3D11Buffer *indexBuffer;
//	//std::vector<Vertex> vertices;
//	int nrVerts, nrIndecies;
//	Vertex *vertices;
//	Index *indecies;
//};

struct TransformData{
	Float3 pos, rot, scale;
};

struct CameraData{
	float fieldOfView; //osvosv
	
};

struct MeshData{
	int nrPos, nrNor, nrUV, nrIPos, nrINor, nrIUV;
	int triangleCount;
	Float3 *positions;
	Float3 *normals;
	Float2 *uvs;

	int *indexPositions;
	int *indexNormals;
	int *indexUVs;
	int *indexTriangles;
	//Material *material; //pekar på ett specifikt material i en vektor av material
	~MeshData(){
		free(positions);
		free(normals);
		free(uvs);
		
		free(indexPositions);
		free(indexNormals);
		free(indexUVs);
		free(indexTriangles);
	}
};

struct MaterialData{
	Float4 diffuse;
	Float4 specular;

	MaterialData(){}
};

struct LightData{

};