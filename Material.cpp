#include "Material.h"

void Material::UpdateCBuffer() {
	//updatesubresource med den nya materialData
	materialCBufferData.diffuse = materialData.diffuse;
	for (int i = 0; i < 3; i++)
	{
		materialCBufferData.color[i] = materialData.color[i];
	}
	for (int i = 0; i < 3; i++)
	{
		materialCBufferData.ambColor[i] = materialData.ambColor[i];
	}
	for (int i = 0; i < 3; i++)
	{
		materialCBufferData.specColor[i] = materialData.specColor[i];
	}
	materialCBufferData.specCosine = materialData.specCosine;
	materialCBufferData.specEccentricity = materialData.specEccentricity;
	materialCBufferData.specRollOff = materialData.specRollOff;

	gDeviceContext->UpdateSubresource(materialCbuffer, 0, NULL, &materialCBufferData, 0, 0);
	gDeviceContext->VSSetConstantBuffers(1, 1, &materialCbuffer);
}
void Material::CreateCBuffer()
{
	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeof(MaterialCBufferData); //kolla så den är 16 byte alligned sen!!
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &materialCBufferData; //ger den startvärde, default, använd updatesubresource sen
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	gDevice->CreateBuffer(&cbDesc, &InitData, &materialCbuffer);
}