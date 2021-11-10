#pragma once
#include<windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include"WinApp.h"
class Input
{
private:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//WindowAPI
	WinApp* winApp = nullptr;
public:
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	void MouseInitialize(WinApp* winApp);

	void Update();
	void MouseUpdate();
	bool PushKey(BYTE keyNumber);
	bool PushMouse(int MouseNumber);
	bool TriggerKey(BYTE keyNumber);
	bool TriggerMouse(int MouseNumber);
private:
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devkeyboard;
	BYTE keyPre[256] = {};
	BYTE key[256] = {};

	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE mouse = { 0 };
	DIMOUSESTATE oldMouse = { 0 };
};