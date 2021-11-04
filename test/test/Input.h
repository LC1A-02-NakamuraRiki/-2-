#pragma once
#include<windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include "WinApp.h"
class Input
{
private:
	WinApp *winApp = nullptr;
public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	void Initialize(WinApp *winApp);

	void Update();
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);
private:
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devkeyboard;
	BYTE keyPre[256] = {};
	BYTE key[256] = {};
};