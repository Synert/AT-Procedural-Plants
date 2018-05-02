#pragma once
#define WIN32
#include <windows.h>
#include "inputclass.h"
#include "cameraclass.h"
#include "graphicsclass.h"
#include "timerclass.h"

const int frameRate = 60;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	CameraClass* m_Camera;
	TwBar* m_Bar;
	TimerClass* m_Timer;
	float dt;

	int oldMousePos[2];
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;