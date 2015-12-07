#include "CameraObj.h"
#include "Transform.h"

void CameraObj::UpdateCBuffer(UINT screenWidth, UINT screenHeight) 
{
	TransformData tDataTemp = transform->transformData;

	XMVECTOR pos = XMVectorSet(tDataTemp.pos.x, tDataTemp.pos.y, tDataTemp.pos.z, 1.0f);
	XMVECTOR rotTemp = XMVectorSet(tDataTemp.rot.x, tDataTemp.rot.y, tDataTemp.rot.z, tDataTemp.rot.w);
	XMVECTOR rot = XMVector3Rotate(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), rotTemp); //+ positionen eller nått sånt, se denna
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view = XMMatrixLookAtRH(pos, rot, up);
	projection = XMMatrixPerspectiveFovRH(
		cameraData.hAngle,
		screenWidth/screenHeight, //aspect ratio?
		1.0f,
		4000
		);

	XMStoreFloat4x4(&cameraCBufferData.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&cameraCBufferData.projection, XMMatrixTranspose(projection));
	
	gDeviceContext->UpdateSubresource(cameraCbuffer, 0, NULL, &cameraCBufferData, 0, 0);
}

void CameraObj::CreateCBuffer() {
	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeof(CameraCBufferData); //kolla så den är 16 byte alligned sen!!
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
/*
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.Usage = D3D11_USAGE_DEFAULT;*/

	// Fill in the subresource data.
	//D3D11_SUBRESOURCE_DATA InitData;
	//InitData.pSysMem = &cameraCBufferData; //ger den startvärde, default, använd updatesubresource sen
	//InitData.SysMemPitch = 0;
	//InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	gDevice->CreateBuffer(&cbDesc, nullptr, &cameraCbuffer);
}