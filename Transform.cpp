#include "Transform.h"

void Transform::UpdateCBuffer()
{
	//updatesubresource med den nya transformData
	XMMATRIX tempWorld = XMMatrixIdentity();

	XMMATRIX tempScale = XMMatrixIdentity();
	XMMATRIX tempRotation = XMMatrixIdentity();
	XMMATRIX tempPosition = XMMatrixIdentity();

	tempScale = XMMatrixScaling(transformData.scale.x, transformData.scale.y, transformData.scale.z);
	//XMMatrixRotationQuaternion anv�nd en quaternion ist�llet! cool stuff, sen b�r det funka
	XMVECTOR rotationQuat = XMVectorSet(transformData.rot.x, transformData.rot.y, transformData.rot.z, transformData.rot.w);
	tempRotation = XMMatrixRotationQuaternion(rotationQuat);
	tempPosition = XMMatrixTranslation(transformData.pos.x, transformData.pos.y, transformData.pos.z);

	tempWorld = tempScale * tempRotation * tempPosition;
	if (parent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&parent->transformCBufferData.world);
		tempWorld = tempWorld * parentWorld;
	}

	XMStoreFloat4x4(&transformCBufferData.world, XMMatrixTranspose(tempWorld));
	//transformdata ligger p� plats 0, material p� 1, osv
	gDeviceContext->UpdateSubresource(transformCBuffer, 0, NULL, &transformCBufferData.world, 0, 0); //skapa en separat struct f�r transformdata som ska in i shadern, world osv
}

void Transform::CreateTransformCBuffer()
{ //gl�m inte parentens skit?
		D3D11_BUFFER_DESC cbDesc = { 0 };
		cbDesc.ByteWidth = sizeof(MaterialData); //kolla s� den �r 16 byte alligned sen!!
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		// Fill in the subresource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &transformData; //ger den startv�rde, default, anv�nd updatesubresource sen
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		// Create the buffer.
		gDevice->CreateBuffer(&cbDesc, &InitData, &transformCBuffer);
}