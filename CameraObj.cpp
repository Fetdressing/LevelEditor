#include "CameraObj.h"
#include "Transform.h"

void CameraObj::UpdateCBuffer() {
	//updatesubresource med den nya materialData
	cameraCBufferData.cData = cameraData;
	cameraCBufferData.tData = transform->transformData;
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

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &cameraCBufferData; //ger den startvärde, default, använd updatesubresource sen
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	gDevice->CreateBuffer(&cbDesc, &InitData, &cameraCbuffer);
}