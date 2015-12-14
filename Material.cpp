#include "Material.h"
#include "WICTextureLoader.h"

void Material::UpdateCBuffer() 
{
	//updatesubresource med den nya materialData
	//hämtar all data som behövs från materialData och store:ar den in constantbufferstructen
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
	//gDeviceContext->PSSetConstantBuffers(1, 1, &materialCbuffer); sätts i render
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

void Material::CreateTexture(char* filePath, ID3D11Resource *texture, ID3D11ShaderResourceView *textureView)
{
	CoInitialize(NULL);
	//
	//std::string tempString(filePath);
	//const wchar_t *filePathWchar = L"sedws";
	//HRESULT br = CreateWICTextureFromFile(gDevice, gDeviceContext, filePathWchar, nullptr, &textureView, 0);
}