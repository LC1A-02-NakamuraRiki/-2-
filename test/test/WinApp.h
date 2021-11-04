#pragma once
#include <windows.h>

class WinApp 
{
public:
	// ウィンドウサイズ
	static const int window_width = 1280;  // 横幅
	static const int window_height = 720;  // 縦幅

private:
	HWND hwnd = nullptr;
public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	void Initialize();

	void Finalize();

	bool ProcessMessage();
public:
	HWND GetHwnd() { return hwnd; }

	HINSTANCE GetHInstance() { return w.hInstance; }
private:
	WNDCLASSEX w{};
};