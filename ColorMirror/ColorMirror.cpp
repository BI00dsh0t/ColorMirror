// ColorMirror.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ColorMirror.h"
#include "LibCorsairRGB\LibCorsairRGB.h"
#include <iostream>
#include <stdlib.h>

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
typedef RZRESULT(*CREATEKEYBOARDCUSTOMEFFECTS)(RZSIZE NumEffects, ChromaSDK::Keyboard::CUSTOM_KEY_EFFECT_TYPE *pCustomEffects, RZEFFECTID *pEffectId);
typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT(*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT(*UNREGISTEREVENTNOTIFICATION)(void);

#ifdef _WIN64
#define CHROMASDKDLL      _T("RzChromaSDK64.dll")
#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
#endif

HWND hWnd = NULL;
HMODULE hModule = NULL;
void initKeyboard();
void initScreenCap();
void closeConnection();
HBITMAP getGetScreenBmp(HDC);
DWORD WINAPI LightThread(LPVOID);
void InitChromaSDK();
void UninitChromaSDK();

int razer = 1;
int chroma = 2;
int type = -1;

int main(){
	hWnd = GetConsoleWindow();
	while (type < 0 || type > 1){
		cout << "What keyboard do you have?" << endl;
		cout << "1: Razer k70 rgb" << endl << "2: Razer Blackwidow Chroma" << endl;
		cin >> type;

		if (type < 0 || type > 1){
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
		CREATEEFFECT CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");		CUSTOM_GRID_EFFECT_TYPE Grid = {};

		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 22; j++){
				Grid.Key[i][j] = RGB(0, 0, 0);
			}
		}
		CreateEffect(BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Grid, nullptr);
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
			green += (lpPixels + count)[0];
			blue += (lpPixels + count)[1];
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
				lcrgb_set_position(x, y, red, blue, green);
			}
		}
		lcrgb_flush_light_buffer();
	}
	else if (type == 2){
		InitChromaSDK();
		CREATEEFFECT CreateEffect = (CREATEEFFECT)GetProcAddress(hModule, "CreateEffect");		CUSTOM_GRID_EFFECT_TYPE Grid = {};

		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 22; j++){
				Grid.Key[i][j] = RGB(red, blue, green);
			}
		}
		CreateEffect(BLACKWIDOW_CHROMA, ChromaSDK::CHROMA_CUSTOM, &Grid, nullptr);
	}

	

	DeleteObject(hBitmap);
	ReleaseDC(NULL, hdc);
	delete[] lpPixels;
	}

DWORD WINAPI LightThread(LPVOID lpParameter)
{

	while (true){
		initScreenCap();
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



