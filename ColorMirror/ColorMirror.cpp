// ColorMirror.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ColorMirror.h"
#include "LibCorsairRGB\LibCorsairRGB.h"
#include <iostream>
#include <stdlib.h>
#include <d3d9.h>
#include <D3dx9tex.h>

#include "RzErrors.h"
#include "RzChromaSDKTypes.h"
#include "RzChromaSDKDefines.h"

using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Mouse;
using namespace std;

typedef RZRESULT(*INIT)(void);
typedef RZRESULT(*UNINIT)(void);
typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEKEYBOARDCUSTOMEFFECTS)(ChromaSDK::Keyboard::CUSTOM_KEY_EFFECT_TYPE *pCustomEffects, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEKEYBOARDCUSTOMGRIDEFFECTS)(ChromaSDK::Keyboard::CUSTOM_GRID_EFFECT_TYPE CustomEffects, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID * pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT(*UNREGISTEREVENTNOTIFICATION)(void);

#ifdef _WIN64
#define CHROMASDKDLL      _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

#define	ErrorMessage(x)		MessageBox(NULL,x,"Error",MB_OK|MB_ICONERROR)
#define BITSPERPIXEL		32

IDirect3D9*			g_pD3D = NULL;
IDirect3DDevice9*	g_pd3dDevice = NULL;
IDirect3DSurface9*	g_pSurface = NULL;
HWND hWnd = NULL;
HMODULE hModule = NULL;
void closeConnection();
HBITMAP getGetScreenBmp(HDC);
DWORD WINAPI LightThread(LPVOID);
void InitChromaSDK();
void UninitChromaSDK();
HRESULT	InitD3D(HWND);
void initDX9ScreenCap();
void initKeyboard();
void initScreenCap();

int razer = 1;
int chroma = 2;
int type = -1;

int main(){
	hWnd = GetConsoleWindow();
	InitD3D(hWnd);
	while (type < 1 || type > 2){
		cout << "What keyboard do you have?" << endl;
		cout << "1: Razer k70 rgb" << endl << "2: Razer Blackwidow Chroma" << endl;
		cin >> type;

		if (type < 1 || type > 2){
			cout << "Invalid input... try again" << endl;
		}
	}

	cout << "You chose the ";

	if (type == 1){
		cout << "Corsair k70 rgb";
		atexit(closeConnection);
	}
	else if (type == 2){
		cout << "Razer Blackwidow Chroma";
		atexit(UninitChromaSDK);
	}
	cout << endl;
	cout << "Press enter to start" << endl;
	system("pause");
	initKeyboard(); 
	
	cout << "Press enter to close" << endl;
	system("pause");

	
	return 0;
}



void initKeyboard(){
	if (type == 1){
		lcrgb_set_verbosity_level(0);
		lcrgb_initialise();
		lcrgb_set_keymap(lcrgb_ANSI_US);
		for (unsigned char key = 0; key < 144; key++){
			lcrgb_set_key_code(key, 0, 0, 0);
		}
	}
	else if (type == 2){
		InitChromaSDK();
		CREATEKEYBOARDCUSTOMGRIDEFFECTS CreateKeyboardCustomEffects = (CREATEKEYBOARDCUSTOMGRIDEFFECTS)GetProcAddress(hModule, "CreateKeyboardCustomGridEffects");
		if (CreateKeyboardCustomEffects == NULL)
		{
			cout << "Effect failed" << endl;
			return;
		}
		CUSTOM_GRID_EFFECT_TYPE Grid = {};		
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 22; j++){
				Grid.Key[i][j] = RGB(255, 0, 0);
			}
		}
		CreateKeyboardCustomEffects(Grid, NULL);
	}


	
	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, LightThread, 0, 0, &myThreadID);
}

void closeConnection(){
	lcrgb_deinitialise();
}

HBITMAP GetScreenBmp(HDC hdc) {
	// Get screen dimensions
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Create compatible DC, create a compatible bitmap and copy the screen using BitBlt()
	HDC hCaptureDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, nScreenWidth, nScreenHeight);
	HGDIOBJ hOld = SelectObject(hCaptureDC, hBitmap);
	BOOL bOK = BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

	SelectObject(hCaptureDC, hOld); // always select the previously selected object once done
	DeleteDC(hCaptureDC);
	return hBitmap;
}

HRESULT	InitD3D(HWND hWnd)
{
	D3DDISPLAYMODE	ddm;
	D3DPRESENT_PARAMETERS	d3dpp;

	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		//ErrorMessage("Unable to Create Direct3D ");
		return E_FAIL;
	}

	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &ddm)))
	{
		//ErrorMessage("Unable to Get Adapter Display Mode");
		return E_FAIL;
	}

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	int horizontal = desktop.right;
	int vertical = desktop.bottom;

	d3dpp.Windowed = true;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferHeight =  vertical = desktop.bottom = ddm.Height;
	d3dpp.BackBufferWidth = horizontal = desktop.right = ddm.Width;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		//ErrorMessage("Unable to Create Device");
		return E_FAIL;
	}

	if (FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_pSurface, NULL)))
	{
		//ErrorMessage("Unable to Create Surface");
		return E_FAIL;
	}

	return S_OK;
}

void initDX9ScreenCap(){
	g_pd3dDevice->GetFrontBufferData(0, g_pSurface);
	LPD3DXBUFFER buffer;
	D3DXSaveSurfaceToFileInMemory(&buffer, D3DXIFF_BMP, g_pSurface, NULL, NULL);
	DWORD imSize = buffer->GetBufferSize();
	void* pix = buffer->GetBufferPointer();

	BYTE* lpPixels = (BYTE*)pix;

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	int horizontal = desktop.right;
	int vertical = desktop.bottom;

	unsigned long red = 0, green = 0, blue = 0, count = 0;

	for (int i = 0; i < horizontal; i++)
	{
		for (int j = 0; j < vertical; j++)
		{
			blue += (lpPixels + count)[2];
			green += (lpPixels + count)[3];
			red += (lpPixels + count)[0];
			count += 4;
		}
	}

	red = red / (horizontal*vertical);
	green = green / (horizontal*vertical);
	blue = blue / (horizontal*vertical);

	if (type == 1) {
		for (int x = 0; x < 22; x++) {
			for (int y = 0; y < 7; y++) {
				lcrgb_set_position(x, y, red, green, blue);
			}
		}
		lcrgb_flush_light_buffer();
	}
	else if (type == 2){
		CREATEKEYBOARDCUSTOMGRIDEFFECTS CreateKeyboardCustomGridEffects = (CREATEKEYBOARDCUSTOMGRIDEFFECTS)GetProcAddress(hModule, "CreateKeyboardCustomGridEffects");
		CUSTOM_GRID_EFFECT_TYPE Grid = {};
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 23; j++){
				Grid.Key[i][j] = RGB(red, green, blue);
			}
		}
		CreateKeyboardCustomGridEffects(Grid, NULL);
	}
	buffer->Release();
}

void initScreenCap(){
	HDC hdc = GetDC(0);

	HBITMAP hBitmap = GetScreenBmp(hdc);

	BITMAPINFO MyBMInfo = { 0 };
	MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

	// Get the BITMAPINFO structure from the bitmap
	if (0 == GetDIBits(hdc, hBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
		//error
	}

	// create the bitmap buffer
	BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

	// Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
	// necessary to read the color table - you might not want this.
	MyBMInfo.bmiHeader.biCompression = BI_RGB;

	// get the actual bitmap buffer
	if (0 == GetDIBits(hdc, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS)) {
		//error
	}

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	int horizontal = desktop.right;
	int vertical = desktop.bottom;

	unsigned long red = 0, green = 0, blue = 0, count = 0;

	for (int i = 0; i < horizontal; i++)
	{
		for (int j = 0; j < vertical; j++)
		{
			blue += (lpPixels + count)[0];
			green += (lpPixels + count)[1];
			red += (lpPixels + count)[2];
			count += 4;
		}
	}

	red = red / (horizontal*vertical);
	green = green / (horizontal*vertical);
	blue = blue / (horizontal*vertical);

	if (type == 1) {
		for (int x = 0; x < 22; x++) {
			for (int y = 0; y < 7; y++) {
				lcrgb_set_position(x, y, red, green, blue);
			}
		}
		lcrgb_flush_light_buffer();
	}
	else if (type == 2){
		CREATEKEYBOARDCUSTOMGRIDEFFECTS CreateKeyboardCustomGridEffects = (CREATEKEYBOARDCUSTOMGRIDEFFECTS)GetProcAddress(hModule, "CreateKeyboardCustomGridEffects");
		CUSTOM_GRID_EFFECT_TYPE Grid = {};
		for (int i = 0; i < 7; i++){
			for (int j = 0; j < 23; j++){
				Grid.Key[i][j] = RGB(red, blue, green);
			}
		}
			CreateKeyboardCustomGridEffects(Grid, NULL);
		}

	

	DeleteObject(hBitmap);
	ReleaseDC(NULL, hdc);
	delete[] lpPixels;
	}

DWORD WINAPI LightThread(LPVOID lpParameter)
{

	while (true){
		//initScreenCap();
		initDX9ScreenCap();
	}
	/*for (int t = 0; t < 50000; t++) {
		for (int x = 0; x < 22; x++) {
			for (int y = 0; y < 7; y++) {
				lcrgb_set_position(x, y, 127 * (1.f + sin(0.3f * (float)x - ((float)t * 0.2f))), 50, 127);
			}
		}
		lcrgb_flush_light_buffer();
		Sleep(50);
	}
	*/
	return 0;
}

void InitChromaSDK()
{
	hModule = LoadLibrary(CHROMASDKDLL);
	if (hModule)
	{
		INIT Init = (INIT)GetProcAddress(hModule, "Init");
		if (Init)
		{
			RZRESULT rzResult = Init();
			if (rzResult == RZRESULT_SUCCESS)
			{
				cout << "Module loaded" << endl;
				REGISTEREVENTNOTIFICATION RegisterEventNotification = (REGISTEREVENTNOTIFICATION)GetProcAddress(hModule, "RegisterEventNotification");
				if (RegisterEventNotification)
				{
					rzResult = RegisterEventNotification(hWnd);
				}
			}
		}
	}
}

void UninitChromaSDK()
{
	if (hModule)
	{
		RZRESULT rzResult = -1;
		UNREGISTEREVENTNOTIFICATION UnregisterEventNotification = (UNREGISTEREVENTNOTIFICATION)GetProcAddress(hModule, "UnregisterEventNotification");
		if (UnregisterEventNotification)
		{
			rzResult = UnregisterEventNotification();
		}

		if (rzResult == RZRESULT_SUCCESS)
		{
			UNINIT UnInit = (UNINIT)GetProcAddress(hModule, "UnInit");
			if (UnInit)
			{
				rzResult = UnInit();
			}
		}

		if (FreeLibrary(hModule))
		{
			hModule = NULL;
		}
	}
}



