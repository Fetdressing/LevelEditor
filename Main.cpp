#include "Main.h"

namespace{
	Main* pMain; //pekare till sjävla applikationen
}
// MSG HANDLER FOR WINMAIN
LRESULT CALLBACK MainWindowProc(HWND hwindow, UINT msg, WPARAM wParam, LPARAM lParam){ //denna används endast för att komma åt RenderEnginens MsgProc() och skicka vidare meddelanden till den
	return pMain->MsgProc(hwindow, msg, wParam, lParam);
}

LRESULT Main::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CHAR: //en tangentbordsknapp har tryckts i
		switch (wParam){
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Main *main = new Main(hInstance, 800, 600);
}

Main::Main(){
	pMain = this;
	Init();
}

Main::Main(HINSTANCE hInstance, UINT scrW, UINT scrH){
	this->hInstance = hInstance;
	this->screenWidth = scrW;
	this->screenHeight = scrH;
	pMain = this;

	Init();
}

Main::~Main(){
	Dealloc();
}

bool Main::Init()
{
	InitVariables(); //klassinstanser och variabler som ska nollställas
	if (!InitWindow()){
		return false; //gick inte att skapa window
	}

	if (!InitDirect3D(hWindow)){
		return false; //gick inte att skapa Direct3D
	}
	SetViewport();
	CreateShaders();
	CreateBuffers();

	mayaLoader = new MayaLoader(gDevice, gDeviceContext, screenWidth, screenHeight);
	Run();	
	return true;
}

int Main::Run(){

	//om allt funkat:
	ShowWindow(hWindow, SW_SHOW);
	MSG msg = { 0 }; //töm alla platser i msg

	// BoundingFrustum b(fpsCam.Proj());//frustum mot quadtree
	//frustum = b;
	//frustum.CreateFromMatrix(frustum, fpsCam.Proj());

	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else{ //applikationen är fortfarande igång			
			Update();
		}
	}
	return static_cast<int>(msg.wParam);
}


void Main::Update(){
	//camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);

	//camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	//camTarget = XMVector3Normalize(camTarget);


	//camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	//camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	//camUp = XMVector3Cross(camForward, camRight);

	//camPosition += moveLeftRight*camRight;
	//camPosition += moveBackForward*camForward;

	//moveLeftRight = 0.0f;
	//moveBackForward = 0.0f;
	//camTarget = camPosition + camTarget;

	//fpsCamLook = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	////Update cam
	//fpsCam.UpdateViewMatrix();
	//CamView = fpsCamLook;
	//CamProjection = fpsCam.Proj();

	////WVP AND OTHERS
	//XMStoreFloat4x4(&WorldData.View, XMMatrixTranspose(CamView));
	//XMStoreFloat4x4(&WorldData.Projection, XMMatrixTranspose(CamProjection));
	//XMStoreFloat4x4(&WorldData.WorldSpace, XMMatrixTranspose(XMMatrixIdentity()));
	//XMStoreFloat4x4(&WorldData.InvWorld, XMMatrixTranspose(XMMatrixInverse(NULL, XMMatrixIdentity())));
	//XMStoreFloat4x4(&WorldData.lightView, XMMatrixTranspose(XMMatrixIdentity()));
	//XMStoreFloat4x4(&WorldData.lightProjection, XMMatrixTranspose(XMMatrixIdentity()));

	mayaLoader->TryReadAMessage();
	Render();
}

void Main::Render(){
	gDeviceContext->OMSetRenderTargets(1, &gBackRufferRenderTargetView, gDepthStencilView);
	gDeviceContext->RSSetViewports(1, &viewport);
	float clearColor[] = { 0.0, 0.3, 0.7f, 1.0f };
	gDeviceContext->ClearRenderTargetView(gBackRufferRenderTargetView, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT32 vertexSize2 = sizeof(float) * 8;
	UINT32 offset2 = 0;
	
	gDeviceContext->IASetInputLayout(defaultInputLayout);
	//gDeviceContext->IASetVertexBuffers(0, 1, &testVertexBuffer, &vertexSize2, &offset2);
	
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->VSSetShader(defaultVS, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(defaultPS, nullptr, 0);
	gDeviceContext->PSSetSamplers(0, 1, &wrap_Sampstate);
	
	//gDeviceContext->Draw(4, 0);

	mayaLoader->DrawScene(); //här så att den får med alla rtv stuff n shiet!

	gSwapChain->Present(0, 0);
}

void Main::InitVariables(){
 //camera grejer var här innan
}

bool Main::InitWindow(){
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.hInstance = hInstance;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; // hur den ska måla ut allt, fast spelar nog ingen roll i vårt fall
	wcex.lpfnWndProc = MainWindowProc;
	//wcex.hCursor = LoadCursor(NULL, IDC_NO); //skoj
	//wcex.hIcon = LoadIcon(NULL, IDI_ERROR); //skoj * 2
	//wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"RENDERENGINECLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)){
		MessageBox(NULL, L"Kunde inte registrera window classen", NULL, NULL);
		return false;
	}

	RECT r = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&r, windowStyle, FALSE); //kommer inte kunna resiza skiten
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	//mainwname = L"Direct3D Projekt";
	hWindow = CreateWindow(L"RENDERENGINECLASS",
		L"Direct3D Projekt", //INTE SÄKER PÅ DETTA, namnet på applikationen blir typ i kinaspråk så venne, kan vara detta
		WS_OVERLAPPEDWINDOW,//Window handlers
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width, height, nullptr, nullptr, hInstance, nullptr);

	if (!hWindow){
		MessageBox(NULL, L"Failed to create window", NULL, NULL);
		return false;
	}


	return true;
}

bool Main::InitDirect3D(HWND hWindow){

	DXGI_SWAP_CHAIN_DESC scd;
	//Describe our SwapChain Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = screenWidth;
	bufferDesc.Height = screenHeight;
	bufferDesc.RefreshRate.Numerator = 144;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc = bufferDesc;
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1; //anti-aliasing
	scd.SampleDesc.Quality = 0;// -kan vi mecka senare men är lite saker som ska göras då
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	// create a device, device context and swap chain using the information in the scd struct
	HRESULT deviceHr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(deviceHr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackRufferRenderTargetView);
		pBackBuffer->Release();

		//DepthBuffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilDesc.Width = screenWidth;
		depthStencilDesc.Height = screenHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;




		deviceHr = gDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);

		deviceHr = gDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &gDepthStencilView);

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc2;
		D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

		// Initialize the description of the stencil state.
		ZeroMemory(&depthStencilDesc2, sizeof(depthStencilDesc2));

		// Set up the description of the stencil state.
		depthStencilDesc2.DepthEnable = true;
		depthStencilDesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc2.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc2.StencilEnable = true;
		depthStencilDesc2.StencilReadMask = 0xFF;
		depthStencilDesc2.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc2.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc2.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc2.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc2.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc2.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		HRESULT state;
		// Create the depth stencil state.
		state = gDevice->CreateDepthStencilState(&depthStencilDesc2, &m_depthStencilState);
		// Set the depth stencil state.
		gDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

		// Clear the second depth stencil state before setting the parameters.
		ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

		// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
		// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the state using the device.
		state = gDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);


		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackRufferRenderTargetView, gDepthStencilView);

		return true; //returnerar att den HAR klarat av att skapa device och swapchain
	}

	return false; //det gick inte att skapa device och swapchain, snyft :'(
}

void Main::SetViewport(){
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void Main::CreateBuffers(){

	struct PlaneVertex
	{
		float x, y, z;
		float ux, uy;
		float nx, ny, nz;
		//float nx2, ny2, nz2;
	}
	PlaneVertices[4] =
	{
		-1.0f, -1.0f, -0.0f,		//v0 
		0.0f, 1.0f,			//t0
		0.0f, 1.0f, 1.0f,


		-1.0f, 1.0f, -0.0f,		//v1
		0.0f, 0.0f,				//t1
		0.0f, 1.0f, 1.0f,

		1.0f, -1.0f, -0.0f,		//v2
		1.0f, 1.0f,			//t2
		0.0f, 1.0f, 1.0f,		//n3
		

		1.0f, 1.0f, -0.0f,		//v3
		1.0f, 0.0f,			//t3
		0.0f, 1.0f, 1.0f	//v3

	};


	// VertexBuffer description
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(PlaneVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = PlaneVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &testVertexBuffer);

}

void Main::CreateShaders(){
	HRESULT ShaderTest = 0;
	HRESULT shaderCreationTest = 0;
	//MAKE SAMPLERS
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	gDevice->CreateSamplerState(&samplerDesc, &wrap_Sampstate);

	D3D11_SAMPLER_DESC samplerDesc2;
	ZeroMemory(&samplerDesc2, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc2.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.MaxAnisotropy = 16;
	gDevice->CreateSamplerState(&samplerDesc2, &clamp_Sampstate);


	//skapa shaders!
	//DEAFULT SHADERS
	//ID3DBlob* layoutblobl = nullptr;

	//create vertex shaders
	ID3DBlob* pVS = nullptr;
	ShaderTest = CompileShader(L"DefaultVS.hlsl", "main", "vs_5_0", &pVS);
	shaderCreationTest = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &defaultVS);

	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ShaderTest = CompileShader(L"DefaultPS.hlsl", "main", "ps_5_0", &pPS);
	ShaderTest = gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &defaultPS);

	// INPUT LAYOUT MÅSTE VARA ANPASSAD TILL VERTEX SHADER

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC default_inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	//CREATE  THE LAYOUTS
	ShaderTest = gDevice->CreateInputLayout(default_inputDesc, ARRAYSIZE(default_inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &defaultInputLayout);

	pPS->Release();
	pVS->Release();

}

HRESULT Main::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void Main::Dealloc(){
	gDevice->Release();
	gDeviceContext->Release();

	delete(mayaLoader);
}
