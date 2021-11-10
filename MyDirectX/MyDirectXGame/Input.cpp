#include "Input.h"
#pragma comment(lib,"dinput8.lib")

bool Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	// DirectInputオブジェクトの生成	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);

	// キーボードデバイスの生成	
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	// 入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	
	// 排他制御レベルのセット
	result = devkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

	return true;
}

void Input::MouseInitialize(WinApp* winApp)
{
	HRESULT result;
	//マウスデバイス生成
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	//入力データ形式のセット
	result = devMouse->SetDataFormat(&c_dfDIMouse);
	//排他制御レベルのセット
	result = devMouse->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Update()
{
	HRESULT result;

	//キーボード情報の取得開始
	result = devkeyboard->Acquire();

	memcpy(keyPre, key, sizeof(key));
	
	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

void Input::MouseUpdate()
{
	HRESULT result;

	//前回のキー入力を保存
	oldMouse = mouse;
	//マウスの情報取得開始
	result = devMouse->Acquire();
	//マウスの入力情報を取得
	result = devMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse);
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber])
	{
		return true;
	}
	return false;
}

bool Input::PushMouse(int MouseNumber)
{
	if (mouse.rgbButtons[MouseNumber])
	{
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (!keyPre[keyNumber] && key[keyNumber]) 
	{
		return true;
	}
	return false;
}

bool Input::TriggerMouse(int MouseNumber)
{
	if (mouse.rgbButtons[MouseNumber] && !oldMouse.rgbButtons[MouseNumber])
	{
		return true;
	}
	return false;
}
