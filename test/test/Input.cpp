#include "Input.h"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


void Input::Initialize(WinApp *winApp)
{
	this->winApp = winApp;
	HRESULT result;

	//初期化（一度だけ行う処理）
	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dinput, nullptr);

	//キーボードデバイスの生成
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);

	//入力データ形式のセット
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);

	//排他制御レベルのセット
	result = devkeyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
}

void Input::Update()
{
	HRESULT result;

	memcpy(keyPre, key, sizeof(key));

	//キーボード情報の取得開始
	result = devkeyboard->Acquire();
	//全キーの入力状態を取得する
	result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber])
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
