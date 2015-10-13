#pragma once
#ifndef MESHDATA_H
#define MESHDATA_H
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
	Float3(){}
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

struct Vertex{
	Float3 pos, nor;
	Float2 uv;

	Vertex(float px, float py, float pz, float nx, float ny, float nz, float cu, float cv){
		pos.Set(px, py, pz);
		nor.Set(nx, ny, nz);
		uv.Set(cu, cv);
	}
	
};