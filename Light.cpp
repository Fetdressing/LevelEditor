#include "Light.h"
#include "Transform.h"

void Light::UpdateCBuffer() {
	//updatesubresource med den nya materialData
	lightCBufferData.lData = lightData;
	lightCBufferData.tData = transform->transformData;
	gDeviceContext->UpdateSubresource(lightCbuffer, 0, NULL, &lightCBufferData, 0, 0);
}

void Light::CreateCBuffer() 
{
	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeof(LightCBufferData); //kolla så den är 16 byte alligned sen!!
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &lightCBufferData; //ger den startvärde, default, använd updatesubresource sen
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	gDevice->CreateBuffer(&cbDesc, &InitData, &lightCbuffer);
}